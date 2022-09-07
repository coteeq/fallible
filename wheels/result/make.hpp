#pragma once

#include <wheels/result/result.hpp>
#include <wheels/error/codes.hpp>

#include <wheels/support/exception.hpp>

namespace wheels {

////////////////////////////////////////////////////////////

namespace detail {

class [[nodiscard]] Failure {
 public:
  explicit Failure(Error error) : error_(std::move(error)) {
  }

  Failure(const Failure&) = delete;
  Failure& operator=(const Failure&) = delete;

  Failure(Failure&&) = delete;
  Failure& operator=(Failure&&) = delete;

  template <typename T>
  operator Result<T>() {
    return Result<T>::Fail(std::move(error_));
  }

 private:
  Error error_;
};

}  // namespace detail

////////////////////////////////////////////////////////////

namespace make_result {

template <typename T>
Result<T> Ok(T&& value) {
  return Result<T>::Ok(std::move(value));
}

template <typename T>
Result<T> Ok(T& value) {
  return Result<T>::Ok(value);
}

template <typename T>
Result<T> Ok(const T& value) {
  return Result<T>::Ok(value);
}

// Usage: make_result::Ok()
Status Ok();

detail::Failure Fail(Error error);

detail::Failure CurrentException();

// PropagateError
//
// Usage:
// Result<Widget> Foo() {
//   Result<Gadget> g = Bar();
//   if (!g.IsOk()) {
//     return PropagateError(g);
//   }
//  ...
// }
//
// Precondition: result.HasError()
template <typename T>
detail::Failure PropagateError(const Result<T>& result) {
  return detail::Failure{result.GetError()};
}

// Convert status code (error or success) to Result
Status ToStatus(std::error_code error);

template <typename T>
Status JustStatus(const Result<T>& result) {
  if (result.IsOk()) {
    return Ok();
  } else {
    return Status::Fail(result.GetError());
  }
}

// For tests
detail::Failure NotSupported();

}  // namespace make_result

}  // namespace wheels
