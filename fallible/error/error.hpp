#pragma once

#include <fallible/error/fwd.hpp>
#include <fallible/context/context.hpp>

#include <vector>

namespace fallible {

class Error {
  friend class detail::ErrorBuilder;

 public:
  int32_t Code() const {
    return code_;
  }

  const Context& context() const {
    return context_;
  }

  std::string Domain() const {
    return context_.Domain();
  }

  std::string Reason() const {
    return context_.Reason();
  }

  wheels::SourceLocation SourceLocation() const {
    return context_.Source();
  }

  std::vector<Error> SubErrors() const {
    return sub_errors_;
  }

  Error SubError() const;

  const Attrs& Attrs() const {
    return context_.Attrs();
  }

  void AddAttr(std::string key, std::string value) {
    context_.AddAttr(std::move(key), std::move(value));
  }

  std::string Describe() const;

  // TODO: Cancellation / errors
  bool IsCancelled() const;

 private:
  Error(detail::ErrorBuilder&);

 private:
  int32_t code_;
  Context context_;
  std::vector<Error> sub_errors_;
};

}  // namespace fallible
