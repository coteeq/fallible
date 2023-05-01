#pragma once

#include <fallible/error/error.hpp>
#include <fallible/error/codes.hpp>
#include <fallible/error/throw.hpp>

#include <fallible/context/make.hpp>
#include <errno.h>

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

struct FromErrno {};

inline detail::ErrorBuilder Err(FromErrno, wheels::SourceLocation loc = wheels::SourceLocation::Current()) {
  auto code = std::make_error_code(static_cast<std::errc>(errno));
  return detail::ErrorBuilder(ErrorCodes::Internal, loc)
    .Domain(code.category().name())
    .Reason(code.message());
}

#define THROW_ERRNO(reason) \
  fallible::ThrowError(fallible::Err(fallible::FromErrno{}, WHEELS_HERE))

#define SYSCALL_VERIFY(cond, reason) \
  if (!(cond)) { THROW_ERRNO(reason); } else {}

//////////////////////////////////////////////////////////////////////

namespace errors {

#define MAKE_ERROR(name) \
inline detail::ErrorBuilder name(wheels::SourceLocation call_site = \
                                       wheels::SourceLocation::Current()) { \
  return Err(ErrorCodes::name).Location(call_site); \
}

MAKE_ERROR(Ok)
MAKE_ERROR(Unknown)
MAKE_ERROR(Disconnected)
MAKE_ERROR(Invalid)
MAKE_ERROR(Cancelled)
MAKE_ERROR(Aborted)
MAKE_ERROR(TimedOut)
MAKE_ERROR(NotFound)
MAKE_ERROR(AlreadyExists)
MAKE_ERROR(Unauthorized)
MAKE_ERROR(Unavailable)
MAKE_ERROR(ResourceExhausted)
MAKE_ERROR(Internal)
MAKE_ERROR(NotSupported)

#undef MAKE_ERROR

}  // namespace errors

}  // namespace fallible
