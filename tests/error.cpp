#include <fallible/error/error.hpp>
#include <fallible/error/codes.hpp>

#include <wheels/test/test_framework.hpp>

#include <iostream>

using fallible::Error;
using fallible::ErrorCodes;
using fallible::Err;

////////////////////////////////////////////////////////////////////////////////

// Test helpers

static Error TimedOut(wheels::SourceLocation loc = wheels::SourceLocation::Current()) {
  return Err(ErrorCodes::TimedOut, loc)
      .Domain("Canonical")
      .Reason("Operation timed out")
      .Done();
}

////////////////////////////////////////////////////////////////////////////////

TEST_SUITE(Error) {
  SIMPLE_TEST(JustWorks) {
    Error error = TimedOut();

    ASSERT_EQ(error.GetCode(), ErrorCodes::TimedOut);
    ASSERT_EQ(error.GetReason(), "Operation timed out");
  }

  SIMPLE_TEST(SubErrors) {
    Error error = Err(ErrorCodes::Internal)
                      .Reason("Internal service error")
                      .Done();

    error.AddSubError(
      Err(123)
            .Reason("Transaction aborted")
            .Done());

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
