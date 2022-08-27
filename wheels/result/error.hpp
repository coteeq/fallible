#pragma once

#include <nlohmann/json.hpp>

#include <wheels/support/source_location.hpp>

namespace wheels {

//////////////////////////////////////////////////////////////////////

class Error {
  using Json = nlohmann::json;

 public:
  int32_t GetCode() const;
  std::string GetDomain() const;

  bool HasReason() const;
  std::string GetReason() const;

  bool HasSourceLocation() const;
  wheels::SourceLocation GetSourceLocation() const;

  void AddSubError(Error&& that);
  std::vector<Error> GetSubErrors() const;
  Error GetSubError() const;

  void AttachContext(std::string_view key, Json value);
  bool HasContext() const;
  Json GetContext() const;

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
  ErrorBuilder(int32_t code, SourceLocation loc);

  ErrorBuilder& Domain(std::string name);
  ErrorBuilder& Reason(std::string descr);
  ErrorBuilder& AttachContext(std::string_view key, Json value);
  ErrorBuilder& AddSubError(Error e);

  Error Done();

 private:
  Json repr_;
};

}  // namespace detail

inline detail::ErrorBuilder Err(int32_t code, SourceLocation loc = SourceLocation::Current()) {
  return detail::ErrorBuilder(code, loc);
}

}  // namespace wheels
