#pragma once

#include <fallible/error/error.hpp>
#include <fallible/error/codes.hpp>

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

//////////////////////////////////////////////////////////////////////

namespace err {

#define MAKE_ERR(name) \
inline detail::ErrorBuilder name() { \
  return Err(ErrorCodes::name); \
}

MAKE_ERR(Ok)
MAKE_ERR(Unknown)
MAKE_ERR(Disconnected)
MAKE_ERR(Invalid)
MAKE_ERR(Cancelled)
MAKE_ERR(Aborted)
MAKE_ERR(TimedOut)
MAKE_ERR(NotFound)
MAKE_ERR(AlreadyExists)
MAKE_ERR(Unauthorized)
MAKE_ERR(Unavailable)
MAKE_ERR(ResourceExhausted)
MAKE_ERR(Internal)
MAKE_ERR(NotSupported)

#undef MAKE_ERR

}  // namespace err

}  // namespace fallible
