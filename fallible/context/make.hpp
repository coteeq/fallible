#pragma once

#include <fallible/context/context.hpp>

#include <fallible/context/tags.hpp>

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

  Builder& AddTag(std::string key, std::string value) {
    tags_.insert_or_assign(key, value);
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

  ContextTags tags_;
};

}  // namespace detail

//////////////////////////////////////////////////////////////////////

inline detail::ContextBuilder Ctx() {
  return {};
}

}  // namespace fallible
