#include <fallible/error/make.hpp>

namespace fallible {

namespace detail {

ErrorBuilder::ErrorBuilder(int32_t code, wheels::SourceLocation loc) {
  code_ = code;
  context_.Location(loc);
}

ErrorBuilder& ErrorBuilder::Domain(std::string name) {
  context_.Domain(name);
  return *this;
}

ErrorBuilder& ErrorBuilder::Reason(std::string descr) {
  context_.Reason(descr);
  return *this;
}

ErrorBuilder& ErrorBuilder::Location(wheels::SourceLocation source) {
  context_.Location(source);
  return *this;
}

ErrorBuilder& ErrorBuilder::AddSubError(Error e) {
  sub_errors_.push_back(std::move(e));
  return *this;
}

ErrorBuilder& ErrorBuilder::Attr(std::string key, std::string value) {
  context_.Attr(std::move(key), std::move(value));
  return *this;
}

Error ErrorBuilder::Done() {
  return Error{*this};
}

}  // namespace detail

}  // namespace fallible
