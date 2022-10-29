#include <fallible/rt/panic.hpp>

#include <fallible/rt/panicker.hpp>

namespace fallible {

namespace rt {

void Panic(const wheels::SourceLocation& where, const std::string& reason) {
  Panicker().Panic(where, reason);
}

}  // namespace rt

}  // namespace fallible
