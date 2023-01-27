#include <fallible/error/error.hpp>

#include <fallible/error/codes.hpp>
#include <fallible/error/make.hpp>

#include <wheels/core/assert.hpp>

#include <sstream>

namespace fallible {

//////////////////////////////////////////////////////////////////////

Error::Error(detail::ErrorBuilder& builder)
    : code_(builder.code_),
      context_(builder.context_.Done()),
      sub_errors_(std::move(builder.sub_errors_)) {
  //
}

Error Error::SubError() const {
  WHEELS_VERIFY(sub_errors_.size() == 1, "Unexpected number of sub-errors: " << sub_errors_.size());
  return sub_errors_.front();
}

bool Error::IsCancelled() const {
  return Code() == ErrorCodes::Cancelled;
}

std::string Error::Describe() const {
  std::stringstream out;

  out << "code = " << Code()
      << " (" << ErrorCodeName(Code()) << ")"
      << ", domain = " << Domain()
      << ", reason = '" << Reason() << "'"
      << ", origin = " << SourceLocation();

  const auto& attrs = Attrs();

  if (!attrs.empty()) {
    out << ", attrs = {";
    size_t index = 0;
    for (const auto& [k, v] : attrs) {
      if (index > 0) {
        out << ", ";
      }
      out << k << " = " << v;
      ++index;
    }
    out << "}" << std::endl;
  }

  return out.str();
}

}  // namespace fallible
