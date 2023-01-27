#pragma once

#include <wheels/core/unit.hpp>

namespace fallible {

// Result = T | Error
template <typename T>
class Result;

// Status = Result<Unit> = Success | Error
using Status = Result<wheels::Unit>;

}  // namespace fallible
