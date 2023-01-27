#pragma once

#include <fallible/error/error.hpp>
#include <fallible/error/throw.hpp>

#include <fallible/result/fwd.hpp>
#include <fallible/result/mappers.hpp>

#include <fallible/rt/panic.hpp>

#include <wheels/core/unit.hpp>

#include <fmt/core.h>

#include <utility>
#include <optional>
#include <string_view>

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

  static Result<T> Ok(T value) {
    return Result(std::move(value));
  }

  static Result<T> Fail(Error error) {
    return Result(std::move(error));
  }

  // Moving

  Result(Result&& that) {
    MoveFrom(std::move(that));
  }

  Result& operator=(Result&& that) {
    Destroy();
    MoveFrom(std::move(that));
    return *this;
  }

  // Copying

  Result(const Result& that) {
    CopyFrom(that);
  }

  Result& operator=(const Result& that) {
    Destroy();
    CopyFrom(that);
  }

  // Dtor

  ~Result() {
    Destroy();
  }

  // Testing

  bool IsOk() const {
    return has_value_;
  }

  bool Failed() const {
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

  // Expect

  // Ignores value, panics on error
  // Usage: result.ExpectOk();
  void ExpectOk(wheels::SourceLocation where = wheels::Here()) {
    ExpectOkImpl(where, "Unexpected error");
  }

  // Ignores value, panics on error
  // Usage: result.ExpectOk("Something bad happens");
  void ExpectOk(std::string_view or_error,
                wheels::SourceLocation where = wheels::Here()) {
    ExpectOkImpl(where, or_error);
  }

  T& ExpectValue(wheels::SourceLocation where = wheels::Here())& {
    ExpectOkImpl(where, "Unexpected error");
    return value_;
  }

  T&& ExpectValue(wheels::SourceLocation where = wheels::Here())&& {
    ExpectOkImpl(where, "Unexpected error");
    return std::move(value_);
  }

  T& ExpectValueOr(std::string_view or_error,
                 wheels::SourceLocation where = wheels::Here())& {
    ExpectOkImpl(where, or_error);
    return value_;
  }

  T&& ExpectValueOr(std::string_view or_error,
                   wheels::SourceLocation where = wheels::Here()) && {
    ExpectOkImpl(where, or_error);
    return std::move(value_);
  }

  // Ignore

  void Ignore(std::string_view /*excuse*/) {
    // No-op
  }

  void TODO(std::string_view /*fix*/) {
    // No-op
  }

  // Error accessors
  // Unsafe: behavior is undefined if result holds a value instead of an error

  bool MatchErrorCode(int expected) const {
    return ErrorCode() == expected;
  }

  const class Error& Error() const {
    return error_;
  }

  int32_t ErrorCode() const {
    return error_.Code();
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

  // Monadic API

  // Result<T> -> Result<U>
  template <ResultMapper<T> F>
  auto Map(F mapper) &&;

  // Result<T> -> U
  template <ResilientMapper<T> F>
  auto Map(F mapper) &&;

  // T -> U
  template <ValueMapper<T> F>
  auto Map(F mapper) &&;

  // T -> Result<U>
  template <FaultyMapper<T> F>
  auto Map(F mapper) &&;

  // Error -> Result<T>
  template <ErrorHandler<T> H>
  Result<T> Recover(H error_handler) &&;

  // ErrorHandler as mapper
  template <ErrorHandler<T> H>
  Result<T> Map(H error_handler) &&;

  // Eat T -> Unit
  template <ValueEater<T> F>
  Status Map(F eater) &&;

  // Eat Result<T> -> Unit
  template <ResultEater<T> F>
  Status Map(F eater) &&;

  // void -> T
  template <VoidMapper F>
  auto Map(F mapper) &&;

  // void -> void
  template <Worker F>
  Status Map(F worker) &&;

  template <Hook F>
  Result<T> Forward(F hook) &&;

  Status JustStatus() &&;

  // Optional

  std::optional<T> ToOptional() &&;

  operator std::optional<T>() && {
    return std::move(*this).ToOptional();
  }

 private:
  template <ResultMapper<T> F>
  auto DoMap(F result_mapper) &&;

 private:
  explicit Result(T && value)
      : has_value_(true),
        value_(std::move(value)) {
  }

  explicit Result(const T& value)
    : has_value_(true),
      value_(value) {
  }

  explicit Result(class Error error)
      : has_value_(false),
        error_(std::move(error)) {
  }

  void MoveFrom(Result&& that) {
    has_value_ = that.has_value_;
    if (has_value_) {
      new (&value_) T(std::move(that.value_));
    } else {
      new (&error_) class Error(std::move(that.error_));
    }
  }

  void CopyFrom(const Result& that) {
    has_value_ = that.has_value_;
    if (has_value_) {
      new (&value_) T(that.value_);
    } else {
      new (&error_) class Error(that.error_);
    }
  }

  void Destroy() {
    if (has_value_) {
      value_.~T();
    } else {
      error_.~Error();
    }
  }

  void ExpectOkImpl(wheels::SourceLocation where, std::string_view or_error) {
    if (!IsOk()) {
      rt::Panic(where, fmt::format("Result::ExpectOk failed: {} ({})", or_error, error_.Describe()));
    }
  }

 private:
  // Tagged union
  bool has_value_;
  union {
    T value_;
    class Error error_;
  };
};

}  // namespace fallible

////////////////////////////////////////////////////////////

// Implementation
#include <fallible/result/result-inl.hpp>
