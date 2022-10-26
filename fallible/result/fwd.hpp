#pragma once

#include <wheels/support/unit.hpp>

namespace fallible {

// Result = T | Error
template <typename T>
class Result;

// Status = Success | Error
using Status = Result<wheels::Unit>;

}  // namespace fallible
