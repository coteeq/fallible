# Fallible

_All happy requests are alike; each unhappy request is unhappy in its own way_.

## Content

- Containers
  - `Error`
  - `Result<T>` = `T` + `Error`
    - `Status` = `Result<Unit>`
- Constructors
  - `Error`: `Err`
  - `Result` / `Status`:
    - `Ok`
    - `Fail`
    - `PropagateError`
    - `ToStatus(std::error_code)`
- Monadic API for `Result<T>`:
  - `Map`
  - `Recover`

[Examples](examples/main.cpp)

## Reading List

- [The Error Model](http://joeduffyblog.com/2016/02/07/the-error-model/) by Joe Duffy
- https://abseil.io/docs/cpp/guides/status

