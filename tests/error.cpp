#include <wheels/result/error.hpp>

#include <wheels/test/test_framework.hpp>

using wheels::Error;

////////////////////////////////////////////////////////////////////////////////

// Test helpers

static std::error_code TimedOut() {
  return std::make_error_code(std::errc::timed_out);
}

static std::exception_ptr RuntimeError() {
  try {
    throw std::runtime_error("Test");
  } catch (...) {
    return std::current_exception();
  }
}

////////////////////////////////////////////////////////////////////////////////

TEST_SUITE(Error) {
  SIMPLE_TEST(Empty) {
    Error error;
    ASSERT_FALSE(error.HasError());
    ASSERT_FALSE(error.HasException());
    ASSERT_FALSE(error.HasErrorCode());
  }

  SIMPLE_TEST(ErrorCode) {
    Error error(TimedOut());
    ASSERT_TRUE(error.HasError());
    ASSERT_TRUE(error.HasErrorCode());
    ASSERT_EQ(error.GetErrorCode().value(), (int) std::errc::timed_out);
    ASSERT_FALSE(error.HasException());
    ASSERT_THROW(error.ThrowIfError(), std::system_error);
  }

  SIMPLE_TEST(Exception) {
    Error error(RuntimeError());
    ASSERT_TRUE(error.HasError());
    ASSERT_FALSE(error.HasErrorCode());
    ASSERT_TRUE(error.HasException());
    ASSERT_THROW(error.ThrowIfError(), std::runtime_error);
  }
}

