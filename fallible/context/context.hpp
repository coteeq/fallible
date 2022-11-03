#pragma once

#include <fallible/context/fwd.hpp>
#include <fallible/context/tags.hpp>

#include <wheels/support/source_location.hpp>

#include <string>

namespace fallible {

class Context {
  friend class detail::ContextBuilder;

  struct Data;

 public:
  std::string Domain() const;
  std::string Reason() const;
  wheels::SourceLocation Source() const;
  const ContextTags& Tags() const;

 private:
  Context(detail::ContextBuilder&);

 private:
  std::shared_ptr<Data> data_;
};

}  // namespace fallible
