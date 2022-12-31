#pragma once

#include <wheels/support/source_location.hpp>

namespace fallible {

// NB: Can represent source location for another program
// on remote server => Allocates memory for file and function names

//////////////////////////////////////////////////////////////////////

class SourceLocation {
 public:
  SourceLocation(wheels::SourceLocation loc)
      : file_(loc.File()),
        function_(loc.Function()),
        line_(loc.Line()) {
  }

  SourceLocation(std::string file, std::string function, int line)
      : file_(std::move(file)),
        function_(std::move(function)),
        line_(std::move(line)) {
  }

  static SourceLocation Current() {
    return wheels::Here();
  }

  const std::string& File() const {
    return file_;
  }

  const std::string& Function() const {
    return function_;
  }

  int Line() const {
    return line_;
  }

 private:
  std::string file_;
  std::string function_;
  int line_;
};

//////////////////////////////////////////////////////////////////////

std::ostream& operator<<(std::ostream& out, const SourceLocation& where);

}  // namespace fallible
