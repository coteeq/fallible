#pragma once

#include <fallible/error/error.hpp>

#include <fallible/context/make.hpp>

namespace fallible {

//////////////////////////////////////////////////////////////////////

namespace detail {

class [[nodiscard]] ErrorBuilder {
  friend fallible::Error;

 public:
  ErrorBuilder(int32_t code, wheels::SourceLocation loc);

  ErrorBuilder& Domain(std::string name);
  ErrorBuilder& Reason(std::string descr);
  ErrorBuilder& Location(wheels::SourceLocation source);
  ErrorBuilder& Location(std::string source);
  ErrorBuilder& Attr(std::string key, std::string value);
  ErrorBuilder& AddSubError(Error e);

  Error Done();

  operator Error() {
    return Done();
  }

 private:
  int32_t code_;
  ContextBuilder context_;
  std::vector<Error> sub_errors_;
};

}  // namespace detail

//////////////////////////////////////////////////////////////////////

inline detail::ErrorBuilder Err(int32_t code, wheels::SourceLocation loc = wheels::SourceLocation::Current()) {
  return detail::ErrorBuilder(code, loc);
}

}  // namespace fallible
