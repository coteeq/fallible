#include <fallible/context/context.hpp>

#include <fallible/context/make.hpp>

namespace fallible {

struct Context::Data {
  std::string reason;
  std::string domain;
  wheels::SourceLocation source;
  ::fallible::Attrs attrs;
};

Context::Context(detail::ContextBuilder& builder) {
  Data data;

  // Fill
  data.reason = builder.reason_;
  data.domain = builder.domain_;
  data.source = builder.source_;
  data.attrs = builder.attrs_;

  data_ = std::make_shared<Data>(std::move(data));
}

std::string Context::Domain() const {
  return data_->domain;
}

std::string Context::Reason() const {
  return data_->reason;
}

wheels::SourceLocation Context::Source() const {
  return data_->source;
}

const Attrs& Context::Attrs() const {
  return data_->attrs;
}

void Context::AddAttr(std::string key, std::string value) {
  data_->attrs.insert_or_assign(std::move(key), std::move(value));
}

}  // namespace fallible
