#include <fallible/context/context.hpp>

#include <fallible/context/make.hpp>

namespace fallible {

struct Context::Data {
  std::string reason;
  std::string domain;
  wheels::SourceLocation source;
  ContextTags tags;
};

Context::Context(detail::ContextBuilder& builder) {
  Data data;

  // Fill
  data.reason = builder.reason_;
  data.domain = builder.domain_;
  data.source = builder.source_;
  data.tags = builder.tags_;

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

const ContextTags& Context::Tags() const {
  return data_->tags;
}

}  // namespace fallible
