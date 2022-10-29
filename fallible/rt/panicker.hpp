#pragma once

#include <wheels/support/source_location.hpp>

#include <string>

namespace fallible {

namespace rt {

//////////////////////////////////////////////////////////////////////

struct IPanicker {
  virtual void Panic(const wheels::SourceLocation& where,
                     const std::string& reason) = 0;
};

//////////////////////////////////////////////////////////////////////

IPanicker& Panicker();

//////////////////////////////////////////////////////////////////////

// For tests / examples
void SetPanicker(IPanicker& impl);

}  // namespace rt

}  // namespace fallible
