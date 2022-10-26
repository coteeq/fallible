#pragma once

#include <nlohmann/json.hpp>

#include <wheels/support/source_location.hpp>

namespace fallible {

//////////////////////////////////////////////////////////////////////

class Error {
  using Json = nlohmann::json;

 public:
  int32_t Code() const;
  std::string Domain() const;

  bool HasReason() const;
  std::string Reason() const;

  bool HasSourceLocation() const;
  wheels::SourceLocation SourceLocation() const;

  Error& AddSubError(Error&& that);
  std::vector<Error> SubErrors() const;
  Error SubError() const;

  Error& AttachContext(std::string_view key, Json value);
  bool HasContext() const;
  Json Context() const;

  Json AsJson() const {
    return repr_;
  }

  std::string Describe() const;

  static Error FromRepr(Json repr) {
    return {std::move(repr)};
  }

 private:
  Error(Json obj) : repr_(std::move(obj)) {
  }

 private:
  Json repr_;
};

//////////////////////////////////////////////////////////////////////

namespace detail {

class [[nodiscard]] ErrorBuilder {
  using Json = nlohmann::json;

 public:
  ErrorBuilder(int32_t code, wheels::SourceLocation loc);

  ErrorBuilder& Domain(std::string name);
  ErrorBuilder& Reason(std::string descr);
  ErrorBuilder& AttachContext(std::string_view key, Json value);
  ErrorBuilder& AddSubError(Error e);

  Error Done();

 private:
  Json repr_;
};

}  // namespace detail

inline detail::ErrorBuilder Err(int32_t code, wheels::SourceLocation loc = wheels::SourceLocation::Current()) {
  return detail::ErrorBuilder(code, loc);
}

}  // namespace fallible
