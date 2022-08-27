#pragma once

// aligned_storage lives here
#include <type_traits>
#include <utility>

#include <optional>
#include <variant>

#include <wheels/support/assert.hpp>
#include <wheels/result/error.hpp>
#include <wheels/result/throw.hpp>

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

namespace wheels {

////////////////////////////////////////////////////////////

namespace detail {

// Low-level storage for value
template <typename T>
class ValueStorage {
  using Storage = typename std::aligned_storage<sizeof(T), alignof(T)>::type;

 public:
  template <typename... Arguments>
  void Construct(Arguments&&... arguments) {
    new (&storage_) T(std::forward<Arguments>(arguments)...);
  }

  void MoveConstruct(T&& that) {
    new (&storage_) T(std::move(that));
  }

  void CopyConstruct(const T& that) {
    new (&storage_) T(that);
  }

  T& RefUnsafe() {
    return *PtrUnsafe();
  }

  const T& ConstRefUnsafe() const {
    return *PtrUnsafe();
  }

  T* PtrUnsafe() {
    return reinterpret_cast<T*>(&storage_);
  }

  const T* PtrUnsafe() const {
    return reinterpret_cast<const T*>(&storage_);
  }

  void Destroy() {
    PtrUnsafe()->~T();
  }

 private:
  Storage storage_;
};

}  // namespace detail

////////////////////////////////////////////////////////////

// Result = Value | Error

template <typename T>
class [[nodiscard]] Result {
 public:
  static_assert(!std::is_reference<T>::value,
                "Reference types are not supported");

  using ValueType = T;

  // Static constructors

  template <typename... Arguments>
  static Result Ok(Arguments && ... arguments) {
    Result result;
    result.value_.Construct(std::forward<Arguments>(arguments)...);
    return result;
  }

  static Result Ok(T && value) {
    return Result(std::move(value));
  }

  static Result Fail(Error error) {
    return Result(std::move(error));
  }

  // Moving

  Result(Result && that) {
    MoveImpl(std::move(that));
  }

  Result& operator=(Result&& that) {
    DestroyValueIfExist();
    MoveImpl(std::move(that));
    return *this;
  }

  // Copying

  Result(const Result& that) {
    CopyImpl(that);
  }

  Result& operator=(const Result& that) {
    DestroyValueIfExist();
    CopyImpl(that);
    return *this;
  }

  // Dtor

  ~Result() {
    DestroyValueIfExist();
  }

  // Testing

  bool HasError() const {
    return error_.has_value();
  }

  bool IsOk() const {
    return !HasError();
  }

  bool HasValue() const {
    return !HasError();
  }

  /* Intentionally disabled
  explicit operator bool() const {
    return IsOk();
  }
  */

  void ThrowIfError() const {
    if (error_) {
      ThrowError(*error_);
    }
  }

  // Ignores value, panics on error
  // Usage: result.ExpectOk();
  void ExpectOk(SourceLocation where = SourceLocation::Current()) {
    ExpectOkImpl(where, "Unexpected error");
  }

  // Ignores value, panics on error
  // Usage: result.ExpectOk("Something bad happens");
  void ExpectOk(const std::string& or_error,
                SourceLocation where = SourceLocation::Current()) {
    ExpectOkImpl(where, or_error);
  }

  T& ExpectValue(SourceLocation where = SourceLocation::Current())& {
    ExpectOkImpl(where, "Unexpected error");
    return value_.RefUnsafe();
  }

  T&& ExpectValue(SourceLocation where = SourceLocation::Current())&& {
    ExpectOkImpl(where, "Unexpected error");
    return std::move(value_.RefUnsafe());
  }

  T& ExpectValueOr(const std::string& or_error,
                 SourceLocation where = SourceLocation::Current())& {
    ExpectOkImpl(where, or_error);
    return value_.RefUnsafe();
  }

  T&& ExpectValueOr(const std::string& or_error,
                   SourceLocation where = SourceLocation::Current()) && {
    ExpectOkImpl(where, or_error);
    return std::move(value_.RefUnsafe());
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
    return *error_;
  }

  int32_t GetErrorCode() const {
    return error_->GetCode();
  }

  // Value accessors

  // Unsafe value getters, use only after IsOk
  // Behavior is undefined if Result does not contain a value

  T& ValueUnsafe() & {
    return value_.RefUnsafe();
  }

  const T& ValueUnsafe() const & {
    return value_.ConstRefUnsafe();
  }

  T&& ValueUnsafe() && {
    return std::move(value_.RefUnsafe());
  }

  // Safe value getters
  // Throws if Result does not contain a value

  T& ValueOrThrow()& {
    ThrowIfError();
    return value_.RefUnsafe();
  }

  const T& ValueOrThrow() const& {
    ThrowIfError();
    return value_.ConstRefUnsafe();
  }

  T&& ValueOrThrow()&& {
    ThrowIfError();
    return std::move(value_.RefUnsafe());
  }

  [[deprecated]] T& Value()& {
    ThrowIfError();
    return ValueUnsafe();
  }

  [[deprecated]] const T& Value() const& {
    ThrowIfError();
    return ValueUnsafe();
  }

  [[deprecated]] T&& Value()&& {
    ThrowIfError();
    return std::move(ValueUnsafe());
  }

  // For templates:

  auto Unwrap() {
    return ValueOrThrow();
  }

  // operator * overloads
  // Unsafe: behavior is undefined if this Result does not contain a value

  T& operator*() & {
    return value_.RefUnsafe();
  }

  const T& operator*() const & {
    return value_.ConstRefUnsafe();
  }

  T&& operator*() && {
    return std::move(value_.RefUnsafe());
  }

  // operator -> overloads
  // Unsafe: behavior is undefined if Result does not contain a value

  T* operator->() {
    return value_.PtrUnsafe();
  }

  const T* operator->() const {
    return value_.PtrUnsafe();
  }

  // Unwrap rvalue Result automatically
  // Do we need this?

  operator T &&()&& {
    return std::move(ValueOrThrow());
  }

 private:
  Result() {
  }

  Result(T && value) {
    value_.MoveConstruct(std::move(value));
  }

  Result(const T& value) {
    value_.CopyConstruct(value);
  }

  Result(Error error) {
    error_ = std::move(error);
  }

  void MoveImpl(Result && that) {
    error_ = std::move(that.error_);
    if (that.HasValue()) {
      value_.MoveConstruct(std::move(that.ValueUnsafe()));
    }
  }

  void CopyImpl(const Result& that) {
    error_ = that.error_;
    if (that.HasValue()) {
      value_.CopyConstruct(that.ValueUnsafe());
    }
  }

  void DestroyValueIfExist() {
    if (IsOk()) {
      value_.Destroy();
    }
  }

  void ExpectOkImpl(SourceLocation where, const std::string& or_error) {
    if (!IsOk()) {
      detail::Panic(where, StringBuilder()
                               << "Result::ExpectOk failed: " << or_error
                               << " (" << error_->Describe() << ")");
    }
  }

 private:
  detail::ValueStorage<T> value_;
  std::optional<Error> error_;
};

////////////////////////////////////////////////////////////

// Status = Result<void> = Success | Error

template <>
class [[nodiscard]] Result<void> {
 public:
  using ValueType = void;

  static Result Ok() {
    return Result{};
  }

  static Result Fail(Error error) {
    return Result(std::move(error));
  }

  // Testing

  bool HasError() const {
    return error_.has_value();
  }

  bool IsOk() const {
    return !HasError();
  }

  /* Intentionally disabled
  explicit operator bool() const {
    return IsOk();
  }
  */

  void ThrowIfError() const {
    if (error_) {
      ThrowError(*error_);
    }
  }

  // Ignores value, panics on error
  // Usage: status.ExpectOk();
  void ExpectOk(SourceLocation where = SourceLocation::Current()) {
    ExpectImpl(where, "Unexpected error");
  }

  // Ignores value, panics on error
  // Usage: status.ExpectOk("Something bad happens");
  void ExpectOk(const std::string& or_error,
                SourceLocation where = SourceLocation::Current()) {
    ExpectImpl(where, or_error);
  }

  void Ignore() {
    // No-op
  }

  void Unwrap() const {
    ThrowIfError();
  }

  // Unsafe
  const Error& GetError() const {
    return *error_;
  }

  bool MatchErrorCode(int expected) const {
    return GetErrorCode() == expected;
  }

  int32_t GetErrorCode() const {
    return error_->GetCode();
  }

 private:
  Result() = default;

  Result(Error error) {
    error_ = std::move(error);
  }

  void ExpectImpl(SourceLocation where, const std::string& or_error) {
    if (!IsOk()) {
      detail::Panic(where, StringBuilder()
                               << "Status::ExpectOk failed: " << or_error
                               << " (" << error_->Describe() << ")");
    }
  }

 private:
  std::optional<Error> error_;
};

using Status = Result<void>;

}  // namespace wheels
