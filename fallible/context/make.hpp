#pragma once

#include <fallible/context/context.hpp>

namespace fallible {

namespace detail {

//////////////////////////////////////////////////////////////////////

class ContextBuilder {
  friend class fallible::Context;

  using Builder = ContextBuilder;
 public:
  Builder& Reason(std::string descr) {
    reason_ = descr;
    return *this;
  }

  Builder& Domain(std::string name) {
    domain_ = name;
    return *this;
  }

  Builder& Location(wheels::SourceLocation source) {
    source_ = source;
    return *this;
  }

  Builder& Here(wheels::SourceLocation source = wheels::SourceLocation::Current()) {
    return Location(source);
  }

  Builder& Attr(std::string key, std::string value) {
    attrs_.insert_or_assign(std::move(key), std::move(value));
    return *this;
  }

  Context Done() {
    return Context{*this};
  }

  operator Context() {
    return Done();
  }

 private:
  std::string reason_ = "?";
  std::string domain_ = "?";
  wheels::SourceLocation source_;

  Attrs attrs_;
};

}  // namespace detail

//////////////////////////////////////////////////////////////////////

inline detail::ContextBuilder Ctx() {
  return {};
}

}  // namespace fallible
