#include <wheels/error/codes.hpp>

namespace wheels {

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
    CASE(ResourceExhaused)
    CASE(Internal)
    CASE(NotSupported)
    default:
      std::abort();
  }
}

}  // namespace wheels