#pragma once

#include <fallible/result/result.hpp>
#include <fallible/error/codes.hpp>
#include <fallible/error/make.hpp>

#include <wheels/support/exception.hpp>
#include <wheels/support/unit.hpp>

namespace fallible {

////////////////////////////////////////////////////////////

// Success

template <typename T>
Result<T> Ok(T value) {
  return Result<T>::Ok(std::move(value));
}

inline Status Ok() {
  return Status::Ok({});
}

////////////////////////////////////////////////////////////

namespace detail {

class [[nodiscard]] Failure {
 public:
  explicit Failure(Error error) : error_(std::move(error)) {
  }

  // Non-copyable
  Failure(const Failure&) = delete;
  Failure& operator=(const Failure&) = delete;

  // Non-movable
  Failure(Failure&&) = delete;
  Failure& operator=(Failure&&) = delete;

  // Implicit conversion to Result<T>
  template <typename T>
  operator Result<T>() {
    return As<T>();
  }

  // Explicit conversion to Result<T>
  template <typename T>
  Result<T> As() {
    return Result<T>::Fail(std::move(error_));
  }

 private:
  Error error_;
};

}  // namespace detail

/*
 * Convert Error to Result<T> automagically deducing value type T
 *
 * Example:
 *
 * fallible::Result<Widget> Foo() {
 *   return fallible::Fail(
 *      fallible::errors::ResourceExhausted());
 * }
 */

detail::Failure Fail(Error error);

////////////////////////////////////////////////////////////

/*
 * Precondition: result.HasError()
 *
 * Example:
 *
 * fallible::Result<Widget> Foo() {
 *   fallible::Result<Gadget> result = Bar();
 *   if (result.HasError()) {
 *     return fallible::PropagateError(result);
 *   }
 *   // Happy path goes here
 * }
 */

template <typename T>
detail::Failure PropagateError(const Result<T>& result) {
  return detail::Failure{result.Error()};
}

////////////////////////////////////////////////////////////

// Erase value type to Unit

template <typename T>
Status JustStatus(const Result<T>& result) {
  if (result.IsOk()) {
    return Ok();
  } else {
    return PropagateError(result);
  }
}

////////////////////////////////////////////////////////////

// Convert std::error_code (error or success) to Status
Status ToStatus(std::error_code error);

////////////////////////////////////////////////////////////

// For tests
detail::Failure NotSupported();

}  // namespace fallible
