#include <fallible/error/error.hpp>
#include <fallible/error/codes.hpp>

#include <wheels/support/assert.hpp>

#include <sstream>

namespace fallible {

//////////////////////////////////////////////////////////////////////

namespace {

nlohmann::json SourceLocationToJson(const wheels::SourceLocation& loc) {
  // clang-format off
  return {
      {"file", loc.File()},
      {"function", loc.Function()},
      {"line", loc.Line()}
  };
  // clang-format on
}

wheels::SourceLocation SourceLocationFromJson(const nlohmann::json& json) {
  // clang-format off
  return {
      json["file"].get<std::string>(),
      json["function"].get<std::string>(),
      json["line"].get<int>(),
  };
  // clang-format on
}

}  // namespace

//////////////////////////////////////////////////////////////////////

int32_t Error::Code() const {
  return repr_["code"].get<int32_t>();
}

std::string Error::Domain() const {
  return repr_["domain"].get<std::string>();
}

bool Error::HasReason() const {
  return repr_.contains("reason");
}

std::string Error::Reason() const {
  return repr_["reason"].get<std::string>();
}

bool Error::HasSourceLocation() const {
  return repr_.contains("where");
}

wheels::SourceLocation Error::SourceLocation() const {
  return SourceLocationFromJson(repr_["where"]);
}

Error& Error::AttachContext(std::string_view key, Json value) {
  repr_["ctx"][key] = value;
  return *this;
}

bool Error::HasContext() const {
  return repr_.contains("ctx");
}

nlohmann::json Error::Context() const {
  return repr_["ctx"];
}

// Sub-errors

Error& Error::AddSubError(Error&& that) {
  repr_["sub_errors"].push_back(std::move(that.repr_));
  return *this;
}

std::vector<Error> Error::SubErrors() const {
  std::vector<Error> sub_errors;

  if (repr_.contains("sub_errors")) {
    auto sub_errors_obj = repr_["sub_errors"];
    for (size_t i = 0; i < sub_errors_obj.size(); ++i) {
      sub_errors.push_back({sub_errors_obj[i]});
    }
  }

  return sub_errors;
}

Error Error::SubError() const {
  auto sub_errors = SubErrors();
  WHEELS_VERIFY(sub_errors.size() == 1, "Unexpected number of sub-errors: " << sub_errors.size());
  return sub_errors.front();
}

std::string Error::Describe() const {
  std::stringstream out;
  out << "code = " << Code()
      << " (" << ErrorCodeName(Code()) << ")"
      << ", domain = " << Domain()
      << ", reason = '" << Reason() << "'"
      << ", origin = " << SourceLocation();

  if (HasContext()) {
    out << ", context = " << Context().dump();
  }

  return out.str();
}

//////////////////////////////////////////////////////////////////////

namespace detail {

ErrorBuilder::ErrorBuilder(int32_t code, wheels::SourceLocation loc) {
  repr_["code"] = code;
  repr_["where"] = SourceLocationToJson(loc);
}

ErrorBuilder& ErrorBuilder::Domain(std::string name) {
  repr_["domain"] = name;
  return *this;
}

ErrorBuilder& ErrorBuilder::Reason(std::string descr) {
  repr_["reason"] = descr;
  return *this;
}

ErrorBuilder& ErrorBuilder::AddSubError(Error e) {
  repr_["sub_errors"].push_back(e.AsJson());
  return *this;
}

ErrorBuilder& ErrorBuilder::AttachContext(std::string_view key, nlohmann::json value) {
  repr_["ctx"][key] = value;
  return *this;
}

Error ErrorBuilder::Done() {
  return Error::FromRepr(std::move(repr_));
}

}  // namespace detail

}  // namespace fallible
