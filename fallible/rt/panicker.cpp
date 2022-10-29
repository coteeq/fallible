#include <fallible/rt/panicker.hpp>

#include <fallible/rt/abort.hpp>

namespace fallible {

namespace rt {

static IPanicker* panicker = &AbortProcess();

void SetPanicker(IPanicker& impl) {
  panicker = &impl;
}

IPanicker& Panicker() {
  return *panicker;
}

}  // namespace rt

}  // namespace fallible