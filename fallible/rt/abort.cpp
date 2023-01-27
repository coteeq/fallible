#include <fallible/rt/abort.hpp>

#include <wheels/core/singleton.hpp>

#include <mutex>
#include <iostream>

namespace fallible {

namespace rt {

struct AbortPanicker : IPanicker {
  void Panic(const wheels::SourceLocation& where,
             const std::string& reason) {
    std::lock_guard locker(mutex_);

    std::cout << "Panicked at " << where << ": " << reason << std::endl;

    std::abort();
  }

 private:
  std::mutex mutex_;
};

IPanicker& AbortProcess() {
  return LeakySingleton<AbortPanicker>();
}

}  // namespace rt

}  // namespace fallible
