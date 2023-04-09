#include <fallible/error/codes.hpp>
#include "wheels/core/panic.hpp"

namespace fallible {

#define CASE(code) case ErrorCodes::code: return #code;

std::string ErrorCodeName(int code) {
  switch (code) {
    CASE(Ok)
    CASE(Unknown)
    CASE(Disconnected)
    CASE(Invalid)
    CASE(Cancelled)
    CASE(Aborted)
    CASE(TimedOut)
    CASE(NotFound)
    CASE(AlreadyExists)
    CASE(Unauthorized)
    CASE(Unavailable)
    CASE(ResourceExhausted)
    CASE(Internal)
    CASE(NotSupported)
    default:
      WHEELS_PANIC("unreachable");
  }
}

}  // namespace fallible
