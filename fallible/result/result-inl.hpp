
#include <fallible/error/codes.hpp>
#include <fallible/error/make.hpp>
#include <fallible/result/ignore.hpp>

#include <wheels/core/exception.hpp>

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

// Resilient mapper

template <typename T>
template <ResilientMapper<T> F>
auto Result<T>::Map(F mapper) && {
  using U = std::invoke_result_t<F, Result<T>>;

  auto result_mapper = [mapper = std::move(mapper)](Result<T> input) mutable -> Result<U> {
    return Result<U>::Ok(mapper(input));
  };

  return std::move(*this).DoMap(std::move(result_mapper));
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
      return Result<U>::Fail(input.Error());
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
      return Result<U>::Fail(input.Error());
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
      return error_handler(input.Error());
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

//////////////////////////////////////////////////////////////////////

// Value eater

template <typename T>
template <ValueEater<T> F>
Status Result<T>::Map(F eater) && {
  auto result_mapper = [eater = std::move(eater)](Result<T> input) mutable -> Status {
    if (input.IsOk()) {
      eater(std::move(*input));
      return Status::Ok({});
    } else {
      return Status::Fail(input.Error());
    }
  };

  return std::move(*this).DoMap(std::move(result_mapper));
}

//////////////////////////////////////////////////////////////////////

// Result eater

template <typename T>
template <ResultEater<T> F>
Status Result<T>::Map(F eater) && {
  auto result_mapper = [eater = std::move(eater)](Result<T> input) mutable -> Status {
    eater(std::move(input));
    return Status::Ok({});
  };

  return std::move(*this).DoMap(std::move(result_mapper));
}

//////////////////////////////////////////////////////////////////////

// Void

// void -> T
template <typename T>
template <VoidMapper F>
auto Result<T>::Map(F mapper) && {
  static_assert(std::same_as<T, wheels::Unit>);

  auto unit_mapper = [mapper = std::move(mapper)](wheels::Unit) mutable {
    return mapper();
  };
  return std::move(*this).Map(std::move(unit_mapper));
}

// void -> void
template <typename T>
template <Worker F>
Status Result<T>::Map(F worker) && {
  static_assert(std::same_as<T, wheels::Unit>);

  auto unit_mapper = [worker = std::move(worker)](wheels::Unit) mutable {
    worker();
    return wheels::Unit{};
  };
  return std::move(*this).Map(std::move(unit_mapper));
}

//////////////////////////////////////////////////////////////////////

// Identity mapper

template <typename T>
template <Hook F>
Result<T> Result<T>::Forward(F hook) && {
  auto result_mapper = [hook = std::move(hook)](Result<T> input) mutable {
    hook();
    return input;
  };
  return std::move(*this).DoMap(std::move(result_mapper));
}

//////////////////////////////////////////////////////////////////////

template <typename T>
Status Result<T>::JustStatus() && {
  if (IsOk()) {
    return Status::Ok({});
  } else {
    return Status::Fail(Error());
  }
}

//////////////////////////////////////////////////////////////////////

template <typename T>
std::optional<T> Result<T>::ToOptional() && {
  if (IsOk()) {
    return std::move(ValueUnsafe());
  } else {
    return std::nullopt;
  }
}

}  // namespace fallible
