#include <fallible/context/context.hpp>

#include <fallible/context/make.hpp>

namespace fallible {

struct Context::Data {
  std::string reason;
  std::string domain;
  fallible::SourceLocation location;
  fallible::Attrs attrs;
};

Context::Context(detail::ContextBuilder& builder) {
  Data data{builder.reason_, builder.domain_, builder.location_, builder.attrs_};
  data_ = std::make_shared<Data>(std::move(data));
}

std::string Context::Domain() const {
  return data_->domain;
}

std::string Context::Reason() const {
  return data_->reason;
}

SourceLocation Context::SourceLocation() const {
  return data_->location;
}

const Attrs& Context::Attrs() const {
  return data_->attrs;
}

bool Context::HasAttr(const std::string& key) const {
  return data_->attrs.contains(key);
}

void Context::AddAttr(std::string key, std::string value) {
  data_->attrs.insert_or_assign(std::move(key), std::move(value));
}

}  // namespace fallible
