#include <fallible/context/location.hpp>

namespace fallible {

std::ostream& operator<<(std::ostream& out, const SourceLocation& where) {
  out << where.File() << ":" << where.Function() << "[Line " << where.Line()
      << "]";
  return out;
}

}  // namespace fallible