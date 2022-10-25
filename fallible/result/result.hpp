#pragma once

#include <fallible/error/error.hpp>
#include <fallible/error/throw.hpp>

#include <wheels/support/unit.hpp>

#include <wheels/support/assert.hpp>

#include <utility>

/* References
 *
 * http://joeduffyblog.com/2016/02/07/the-error-model/
 *
 * C++:
 * https://www.boost.org/doc/libs/1_72_0/libs/outcome/doc/html/index.html
 * https://github.com/llvm-mirror/llvm/blob/master/include/llvm/Support/ErrorOr.h
 * https://github.com/facebook/folly/blob/master/folly/Try.h
 * https://abseil.io/docs/cpp/guides/status
 * https://github.com/TartanLlama/expected
 * https://www.youtube.com/watch?v=CGwk3i1bGQI
 * http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2017/p0762r0.pdf
 *
 * Rust:
 * https://doc.rust-lang.org/book/ch09-02-recoverable-errors-with-result.html
 */

namespace fallible {

////////////////////////////////////////////////////////////

// Result = Value | Error

template <typename T>
class [[nodiscard]] Result {
 public:
  static_assert(!std::is_reference<T>::value,
                "Reference types are not supported");

  using ValueType = T;

  // Static constructors

  static Result Ok(T value) {
    return Result(std::move(value));
  }

  static Result Fail(Error error) {
    return Result(std::move(error));
  }

  // Moving

  Result(Result&& that) {
    MoveImpl(std::move(that));
  }

  Result& operator=(Result&& that) {
    Destroy();
    MoveImpl(std::move(that));
    return *this;
  }

  // Copying

  Result(const Result& that) {
    CopyImpl(that);
  }

  Result& operator=(const Result& that) {
    Destroy();
    CopyImpl(that);
  }

  // Dtor

  ~Result() {
    Destroy();
  }

  // Testing

  bool IsOk() const {
    return has_value_;
  }

  bool HasError() const {
    return !IsOk();
  }

  /* Intentionally disabled
  explicit operator bool() const {
    return IsOk();
  }
  */

  void ThrowIfError() const {
    if (!has_value_) {
      ThrowError(error_);
    }
  }

  // Ignores value, panics on error
  // Usage: result.ExpectOk();
  void ExpectOk(wheels::SourceLocation where = wheels::SourceLocation::Current()) {
    ExpectOkImpl(where, "Unexpected error");
  }

  // Ignores value, panics on error
  // Usage: result.ExpectOk("Something bad happens");
  void ExpectOk(const std::string& or_error,
                wheels::SourceLocation where = wheels::SourceLocation::Current()) {
    ExpectOkImpl(where, or_error);
  }

  T& ExpectValue(wheels::SourceLocation where = wheels::SourceLocation::Current())& {
    ExpectOkImpl(where, "Unexpected error");
    return value_;
  }

  T&& ExpectValue(wheels::SourceLocation where = wheels::SourceLocation::Current())&& {
    ExpectOkImpl(where, "Unexpected error");
    return std::move(value_);
  }

  T& ExpectValueOr(const std::string& or_error,
                 wheels::SourceLocation where = wheels::SourceLocation::Current())& {
    ExpectOkImpl(where, or_error);
    return value_;
  }

  T&& ExpectValueOr(const std::string& or_error,
                   wheels::SourceLocation where = wheels::SourceLocation::Current()) && {
    ExpectOkImpl(where, or_error);
    return std::move(value_);
  }

  void Ignore() {
    // No-op
  }

  // Error accessors
  // Unsafe: behavior is undefined if result holds a value instead of an error

  bool MatchErrorCode(int expected) const {
    return GetErrorCode() == expected;
  }

  const Error& GetError() const {
    return error_;
  }

  int32_t GetErrorCode() const {
    return error_.GetCode();
  }

  // Value accessors

  // Unsafe value getters, use only after IsOk
  // Behavior is undefined if Result does not contain a value

  T& ValueUnsafe() & {
    return value_;
  }

  const T& ValueUnsafe() const & {
    return value_;
  }

  T&& ValueUnsafe() && {
    return std::move(value_);
  }

  // Safe value getters
  // Throws if Result does not contain a value

  T& ValueOrThrow()& {
    ThrowIfError();
    return value_;
  }

  const T& ValueOrThrow() const& {
    ThrowIfError();
    return value_;
  }

  T&& ValueOrThrow()&& {
    ThrowIfError();
    return std::move(value_);
  }

  // For templates:

  auto Unwrap() {
    return ValueOrThrow();
  }

  // operator * overloads
  // Unsafe: behavior is undefined if this Result does not contain a value

  T& operator*() & {
    return value_;
  }

  const T& operator*() const & {
    return value_;
  }

  T&& operator*() && {
    return std::move(value_);
  }

  // operator -> overloads
  // Unsafe: behavior is undefined if Result does not contain a value

  T* operator->() {
    return &value_;
  }

  const T* operator->() const {
    return &value_;
  }

  // Unwrap rvalue Result automatically

//  operator T &&()&& {
//    return std::move(value_);
//  }

 private:
  Result(T && value)
      : has_value_(true),
        value_(std::move(value)) {
  }

  Result(const T& value)
    : has_value_(true),
      value_(value) {
  }

  Result(Error error)
      : has_value_(false),
        error_(std::move(error)) {
  }

  void MoveImpl(Result&& that) {
    has_value_ = that.has_value_;
    if (has_value_) {
      new (&value_) T(std::move(that.value_));
    } else {
      new (&error_) Error(std::move(that.error_));
    }
  }

  void CopyImpl(const Result& that) {
    has_value_ = that.has_value_;
    if (has_value_) {
      new (&value_) T(that.value_);
    } else {
      new (&error_) Error(that.error_);
    }
  }

  void Destroy() {
    if (has_value_) {
      value_.~T();
    } else {
      error_.~Error();
    }
  }

  void ExpectOkImpl(wheels::SourceLocation where, const std::string& or_error) {
    if (!IsOk()) {
      wheels::detail::Panic(where, wheels::StringBuilder()
                               << "Result::ExpectOk failed: " << or_error
                               << " (" << error_.Describe() << ")");
    }
  }

 private:
  bool has_value_;
  union {
    T value_;
    Error error_;
  };
};

////////////////////////////////////////////////////////////

// Status = Result<Unit> = Success | Error

using Status = Result<wheels::Unit>;

}  // namespace fallible
