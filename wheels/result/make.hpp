#pragma once

#include <wheels/result/result.hpp>
#include <wheels/result/error_codes.hpp>

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
    return Result<T>::Fail(error_);
  }

 private:
  Error error_;
};

}  // namespace detail

////////////////////////////////////////////////////////////

namespace detail {

template <typename R>
struct Invoker {
  template <typename F, typename... Args>
  static Result<R> Invoke(F&& f, Args&&... args) {
    try {
      return Result<R>::Ok(f(std::forward<Args>(args)...));
    } catch (...) {
      return Result<R>::Fail(
          Err(ErrorCodes::Unknown)
              .Domain("Canonical")
              .Reason(CurrentExceptionMessage())
              .Done());
    }
  }
};

template <>
struct Invoker<void> {
  template <typename F, typename... Args>
  static Status Invoke(F&& f, Args&&... args) {
    try {
      f(std::forward<Args>(args)...);
      return Status::Ok();
    } catch (...) {
      return Status::Fail(
          Err(ErrorCodes::Unknown)
              .Domain("Canonical")
              .Reason(CurrentExceptionMessage())
              .Done());
    }
  }
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

template <typename F, typename... Args>
auto Invoke(F&& f, Args&&... args) {
  using R = decltype(f(std::forward<Args>(args)...));
  return detail::Invoker<R>::Invoke(std::forward<F>(f),
                                    std::forward<Args>(args)...);
}

//// Make result with exception
//template <typename E, typename... Args>
//detail::Failure Throw(Args&&... args) {
//  try {
//    throw E(std::forward<Args>(args)...);
//  } catch (const E& e) {
//    return detail::Failure{std::current_exception()};
//  }
//}

// Produce std::errc::not_supported error
// For tests
detail::Failure NotSupported();

}  // namespace make_result

}  // namespace wheels
