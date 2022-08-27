#pragma once

#include <wheels/result/error.hpp>

namespace wheels {

class ErrorException : public std::runtime_error {
 public:
  ErrorException(Error e)
      : std::runtime_error(e.Describe()),
        error_(std::move(e)) {
  }

  const Error& GetError() const {
    return error_;
  }

 private:
  Error error_;
};

[[noreturn]] inline void ThrowError(Error e) {
  throw ErrorException{std::move(e)};
}

}  // namespace wheels
