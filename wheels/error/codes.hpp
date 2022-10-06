#pragma once

#include <string>

namespace wheels {

//////////////////////////////////////////////////////////////////////

// Canonical error codes

// https://abseil.io/docs/cpp/guides/status-codes
// https://abseil.io/docs/cpp/guides/status#canonical_errors

//////////////////////////////////////////////////////////////////////

struct ErrorCodes {
  enum _ {
    Ok = 0,
    Unknown = 1,
    Disconnected = 2,
    Invalid = 3,
    Cancelled = 4,
    Aborted = 5,
    TimedOut = 6,
    NotFound = 7,
    AlreadyExists = 8,
    Unauthorized = 9,
    Unavailable = 10,
    ResourceExhaused = 11,
    Internal = 12,
    NotSupported = 13,
  };
};

//////////////////////////////////////////////////////////////////////

std::string ErrorCodeName(int code);

//////////////////////////////////////////////////////////////////////

}  // namespace wheels
