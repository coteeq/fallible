#pragma once

#include <fallible/result/result.hpp>
#include <fallible/error/codes.hpp>

#include <wheels/support/exception.hpp>
#include <wheels/support/unit.hpp>

namespace fallible {

////////////////////////////////////////////////////////////

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

Status Ok();

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
  Result<T> As() {
    return Result<T>::Fail(std::move(error_));
  }

  template <typename T>
  operator Result<T>() {
    return As<T>();
  }

 private:
  Error error_;
};

}  // namespace detail

detail::Failure Fail(Error error);

////////////////////////////////////////////////////////////

/*
 * Precondition: result.HasError()
 *
 * Example:
 *
 * Result<Widget> Foo() {
 *   Result<Gadget> result = Bar();
 *   if (result.HasError()) {
 *     return PropagateError(result);
 *   }
 *   // Happy path goes here
 * }
 */

template <typename T>
detail::Failure PropagateError(const Result<T>& result) {
  return detail::Failure{result.GetError()};
}

////////////////////////////////////////////////////////////

// Convert std::error_code (error or success) to Status
Status ToStatus(std::error_code error);

// Erase value type
template <typename T>
Status JustStatus(const Result<T>& result) {
  if (result.IsOk()) {
    return Ok();
  } else {
    return PropagateError(result);
  }
}

////////////////////////////////////////////////////////////

// For tests
detail::Failure NotSupported();

}  // namespace fallible
