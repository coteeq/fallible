#include <fallible/error/error.hpp>
#include <fallible/error/codes.hpp>
#include <fallible/error/make.hpp>

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

    ASSERT_EQ(error.Code(), ErrorCodes::TimedOut);
    ASSERT_EQ(error.Reason(), "Operation timed out");
  }

  SIMPLE_TEST(SubErrors) {
    auto builder = Err(ErrorCodes::Internal)
                       .Reason("Internal service error");

    builder.AddSubError(
      Err(123)
            .Reason("Transaction aborted")
            .Done());

    auto error = builder.Done();

    std::cout << error.AsJson().dump(1, ' ') << std::endl;

    auto sub_error = error.SubError();

    ASSERT_EQ(sub_error.Code(), 123);
  }

  SIMPLE_TEST(ToJson) {
    auto json = TimedOut().AsJson();

    ASSERT_EQ(json["code"].get<int32_t>(), ErrorCodes::TimedOut);
    ASSERT_EQ(json["context"]["reason"].get<std::string>(), "Operation timed out");
    ASSERT_TRUE(json["context"].contains("source"));

    std::cout << "json = " << json << std::endl;
  }
}
