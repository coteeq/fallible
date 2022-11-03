#include <fallible/error/error.hpp>

#include <fallible/error/codes.hpp>
#include <fallible/error/make.hpp>

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

  const auto& tags = Tags();

  if (!tags.empty()) {
    out << ", context = {";
    size_t index = 0;
    for (const auto& [k, v] : tags) {
      if (index > 0) {
        out << ", ";
      }
      out << k << " = " << v;
      ++index;
    }
  }

  return out.str();
}

//////////////////////////////////////////////////////////////////////

nlohmann::json Error::AsJson() const {
  nlohmann::json context_json = {
    {"domain", context_.Domain()},
    {"reason", context_.Reason()},
    {"source", SourceLocationToJson(context_.Source())}
  };

  const auto& tags = context_.Tags();

  if (!tags.empty()) {
    nlohmann::json tags_json = {};
    for (const auto& [k, v] : tags) {
      tags_json[k] = v;
    }
    context_json.push_back({"tags", tags_json});
  }

  std::vector<nlohmann::json> sub_errors_json;
  for (const auto& error : sub_errors_) {
    sub_errors_json.push_back(error.AsJson());
  }

  return {
    {"code", code_},
    {"context", context_json},
    {"sub_errors", sub_errors_json}
  };
}

Error Error::FromRepr(nlohmann::json repr) {
  auto builder = Err(repr["code"])
      .Domain(repr["context"]["domain"])
      .Reason(repr["context"]["reason"])
      .Location(SourceLocationFromJson(repr["context"]["source"]));

  if (repr["context"].contains("tags")) {
    auto tags = repr["context"]["tags"];
    for (const auto& [k, v] : tags.items()) {
      builder.AddTag(k, v);
    }
  }

  for (const auto& sub : repr["sub_errors"]) {
    builder.AddSubError(Error::FromRepr(sub));
  }

  return builder.Done();
}

}  // namespace fallible
