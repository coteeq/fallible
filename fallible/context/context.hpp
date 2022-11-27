#pragma once

#include <fallible/context/fwd.hpp>
#include <fallible/context/location.hpp>
#include <fallible/context/attrs.hpp>

#include <string>
#include <memory>

namespace fallible {

class Context {
  friend class detail::ContextBuilder;

  struct Data;

 public:
  std::string Domain() const;
  std::string Reason() const;
  SourceLocation SourceLocation() const;
  const Attrs& Attrs() const;

  bool HasAttr(const std::string& key) const;
  void AddAttr(std::string key, std::string value);

 private:
  Context(detail::ContextBuilder&);

 private:
  std::shared_ptr<Data> data_;
};

}  // namespace fallible
