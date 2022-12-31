#pragma once

#include <wheels/support/source_location.hpp>

#include <string>

namespace fallible {

namespace rt {

struct IPanicker {
  virtual void Panic(const wheels::SourceLocation& where,
                     const std::string& reason) = 0;
};

}  // namespace rt

}  // namespace fallible
