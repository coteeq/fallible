#pragma once

#include <wheels/core/source_location.hpp>

#include <string>

namespace fallible {

namespace rt {

// Panic on unrecoverable error
void Panic(const wheels::SourceLocation& where, const std::string& reason);

}  // namespace rt

}  // namespace fallible
