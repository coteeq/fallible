
#include <fallible/error/codes.hpp>
#include <fallible/result/ignore.hpp>

#include <wheels/support/exception.hpp>

namespace fallible {

//////////////////////////////////////////////////////////////////////

template <typename T>
template <ResultMapper<T> F>
auto Result<T>::DoMap(F mapper) && {
  using ResultU = std::invoke_result_t<F, Result<T>>;

  try {
    return mapper(std::move(*this));
  } catch (IgnoreThisException&) {
    // Ignore
    throw;
  } catch (...) {
    // Unhandled user exception
    return ResultU::Fail(
        Err(ErrorCodes::Unknown)
            .Domain("Fallible")
            .Reason(std::string("Unhandled exception in user mapper: ") + wheels::CurrentExceptionMessage())
            .Done());
  }
}

//////////////////////////////////////////////////////////////////////

// Result mapper

template <typename T>
template <ResultMapper<T> F>
auto Result<T>::Map(F mapper) && {
  return std::move(*this).DoMap(std::move(mapper));
}

//////////////////////////////////////////////////////////////////////

// Value mapper

template <typename T>
template <ValueMapper<T> F>
auto Result<T>::Map(F mapper) && {
  using U = std::invoke_result_t<F, T>;

  auto result_mapper = [mapper = std::move(mapper)](Result<T> input) mutable -> Result<U> {
    if (input.IsOk()) {
      return Result<U>::Ok(mapper(*input));
    } else {
      return Result<U>::Fail(input.GetError());
    }
  };

  return std::move(*this).DoMap(std::move(result_mapper));
}

//////////////////////////////////////////////////////////////////////

// Faulty mapper

template <typename T>
template <FaultyMapper<T> F>
auto Result<T>::Map(F mapper) && {
  using ResultU = std::invoke_result_t<F, T>;
  using U = typename ResultU::ValueType;

  auto result_mapper = [mapper = std::move(mapper)](Result<T> input) mutable -> Result<U> {
    if (input.IsOk()) {
      return mapper(*input);
    } else {
      return Result<U>::Fail(input.GetError());
    }
  };

  return std::move(*this).DoMap(std::move(result_mapper));
}

//////////////////////////////////////////////////////////////////////

// Recover

template <typename T>
template <ErrorHandler<T> H>
Result<T> Result<T>::Recover(H error_handler) && {
  auto result_mapper = [error_handler = std::move(error_handler)](Result<T> input) mutable -> Result<T> {
    if (input.IsOk()) {
      return input;
    } else {
      return error_handler(input.GetError());
    }
  };

  return std::move(*this).DoMap(std::move(result_mapper));
}

// Recover as Map

template <typename T>
template <ErrorHandler<T> H>
Result<T> Result<T>::Map(H error_handler) && {
  return std::move(*this).Recover(std::move(error_handler));
}

}  // namespace fallible
