#include <fallible/rt/panic.hpp>

#include <fallible/rt/panicker.hpp>
#include <fallible/rt/abort.hpp>

#include <compass/locate.hpp>

namespace fallible {

namespace rt {

static IPanicker& Panicker() {
  auto* panicker = compass::Map().TryLocate<IPanicker>();

  if (panicker != nullptr) {
    return *panicker;
  } else {
    return AbortProcess();
  }
}

void Panic(const wheels::SourceLocation& where, const std::string& reason) {
  Panicker().Panic(where, reason);
}

}  // namespace rt

}  // namespace fallible
