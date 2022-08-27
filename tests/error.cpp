#include <wheels/result/error.hpp>
#include <wheels/result/error_codes.hpp>

#include <wheels/test/test_framework.hpp>

#include <iostream>

using wheels::Error;
using wheels::ErrorCodes;

////////////////////////////////////////////////////////////////////////////////

// Test helpers

static Error TimedOut(wheels::SourceLocation loc = wheels::SourceLocation::Current()) {
  return Error::Make(ErrorCodes::TimedOut, "Operation timed out", loc);
}

////////////////////////////////////////////////////////////////////////////////

TEST_SUITE(Error) {
  SIMPLE_TEST(JustWorks) {
    Error error = TimedOut();

    ASSERT_EQ(error.GetCode(), ErrorCodes::TimedOut);
    ASSERT_EQ(error.GetReason(), "Operation timed out");
  }

  SIMPLE_TEST(SubErrors) {
    Error error = Error::Make(ErrorCodes::Internal, "Internal service error");
    error.AddSubError(
      Error::Make(123, "Transaction aborted"));

    std::cout << error.AsJson().dump(1, ' ') << std::endl;

    auto sub_error = error.GetSubError();

    ASSERT_EQ(sub_error.GetCode(), 123);
  }

  SIMPLE_TEST(ToJson) {
    auto json = TimedOut().AsJson();

    ASSERT_EQ(json["code"].get<int32_t>(), ErrorCodes::TimedOut);
    ASSERT_EQ(json["reason"].get<std::string>(), "Operation timed out");
    ASSERT_TRUE(json.contains("where"));

    std::cout << "json = " << json << std::endl;
  }
}
