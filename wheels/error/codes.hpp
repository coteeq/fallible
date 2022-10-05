#pragma once

namespace wheels {

//////////////////////////////////////////////////////////////////////

// Canonical error codes

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
    Internal = 10,
    NotSupported = 11,
    Unavailable = 12,
  };
};

}  // namespace wheels
