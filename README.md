# Fallible

_All happy requests are alike; each unhappy request is unhappy in its own way_.

## Content

- Containers
  - `Context`
  - `Error` = `int32_t` code + `Context`
  - `Result<T>` = `T` + `Error`
    - `Status` = `Result<Unit>`
- Constructors
  - `Context`: `Ctx`
  - `Error`: `Err`
  - `Result` / `Status`:
    - `Ok`
    - `Fail`
    - `PropagateError`
    - `ToStatus(std::error_code)`
- Monadic API for `Result<T>`:
  - Combinators
    - `Map`
    - `Recover`
  - [Mappers](fallible/result/mappers.hpp)

[Examples](examples/main.cpp)

## Reading List

- [The Error Model](http://joeduffyblog.com/2016/02/07/the-error-model/) by Joe Duffy
- https://abseil.io/docs/cpp/guides/status
