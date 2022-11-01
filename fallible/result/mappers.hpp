#pragma once

#include <fallible/error/error.hpp>
#include <fallible/result/fwd.hpp>

#include <wheels/support/concepts.hpp>

#include <type_traits>
#include <concepts>

namespace fallible {

// clang-format off

// ValueHandler

template <typename F, typename T>
concept ValueHandler = requires (F f, T value) {
  f(std::move(value));
};

// Result mapper: Result<T> -> Result<U>

template <typename F, typename T>
concept ResultMapper = requires (F f, Result<T> result) {
  { f(std::move(result)) } -> wheels::InstantiationOf<Result>;
};

// Error handler: Error -> Result<T>

template <typename H, typename T>
concept ErrorHandler = requires (H handler, Error error) {
  { handler(error) } -> std::same_as<Result<T>>;
};

// Faulty value mapper: T -> Result<U>

template <typename F, typename T>
concept FaultyMapper = requires (F mapper, T value) {
  { mapper(std::move(value)) } -> wheels::InstantiationOf<Result>;
};

// Value eater

template <typename F, typename T>
concept ValueEater = requires (F mapper, T value) {
  { mapper(std::move(value)) } -> std::same_as<void>;
};

// Result eater

template <typename F, typename T>
concept ResultEater = requires (F mapper, Result<T> value) {
  { mapper(std::move(value)) } -> std::same_as<void>;
};

// ValueMapper: T -> U

// Exclude void, Result from output types
template <typename F, typename T>
concept ValueMapper = ValueHandler<F, T> && !FaultyMapper<F, T> && !ValueEater<F, T>;

// clang-format on

}  // namespace await::futures
