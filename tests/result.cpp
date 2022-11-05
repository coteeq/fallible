#include <fallible/result/result.hpp>
#include <fallible/result/make.hpp>
#include <fallible/result/type_traits.hpp>

#include <wheels/test/test_framework.hpp>

using fallible::Error;
using fallible::ErrorCodes;

using fallible::Result;
using fallible::Status;

// Constructors

using fallible::Ok;
using fallible::Fail;

////////////////////////////////////////////////////////////////////////////////

static Error TimedOut() {
  return fallible::Err(ErrorCodes::TimedOut)
      .Reason("Operation timed out")
      .Done();
}

////////////////////////////////////////////////////////////////////////////////

class TestClass {
 public:
  TestClass(std::string message)
      : message_(std::move(message)) {
    ++object_count_;
  }

  TestClass(const TestClass& that)
      : message_(that.message_) {
  }

  TestClass& operator =(const TestClass& that) {
    message_ = that.message_;
    return *this;
  }

  TestClass(TestClass&& that)
      : message_(std::move(that.message_)) {
    ++object_count_;
  }

  TestClass& operator =(TestClass&& that) {
    message_ = std::move(that.message_);
    return *this;
  }

  ~TestClass() {
    --object_count_;
  }

  const std::string& Message() const {
    return message_;
  }

  static int ObjectCount() {
    return object_count_;
  }

 private:
  TestClass();

 private:
  std::string message_;
  static int object_count_;
};

int TestClass::object_count_ = 0;

////////////////////////////////////////////////////////////////////////////////

Result<std::vector<int>> MakeVector(size_t size) {
  std::vector<int> ints;
  ints.reserve(size);
  for (size_t i = 0; i < size; ++i) {
    ints.push_back(i);
  }
  return fallible::Ok(std::move(ints));
}

Result<std::string> MakeError() {
  return fallible::Fail(TimedOut());
}

////////////////////////////////////////////////////////////////////////////////


TEST_SUITE(Result) {

  SIMPLE_TEST(Ok) {
    static const std::string kMessage = "Hello";

    auto result = Result<TestClass>::Ok(kMessage);
    ASSERT_TRUE(result.IsOk());
    ASSERT_FALSE(result.Failed());
    result.ThrowIfError();  // Nothing happens

    ASSERT_EQ(result.ValueUnsafe().Message(), kMessage);
    ASSERT_EQ((*result).Message(), "Hello");

    ASSERT_EQ(result->Message(), kMessage);
  }

  SIMPLE_TEST(ObjectCount) {
    {
      auto result = fallible::Ok<TestClass>({"Hi"});
      std::cout << TestClass::ObjectCount() << std::endl;
      ASSERT_EQ(TestClass::ObjectCount(), 1);
    }
    ASSERT_EQ(TestClass::ObjectCount(), 0);

    {
      auto result = Result<TestClass>::Fail(TimedOut());
      ASSERT_EQ(TestClass::ObjectCount(), 0);
    }
    ASSERT_EQ(TestClass::ObjectCount(), 0);
  }

  SIMPLE_TEST(OkCopyCtor) {
    static const std::string kMessage = "Copy me";

    auto result = Result<TestClass>::Ok(TestClass(kMessage));
    ASSERT_TRUE(result.IsOk());
    ASSERT_EQ(result->Message(), kMessage);
  }

  SIMPLE_TEST(Error) {
    auto result = Result<TestClass>::Fail(TimedOut());

    ASSERT_FALSE(result.IsOk());
    ASSERT_TRUE(result.Failed());

    int32_t error_code = result.ErrorCode();
    ASSERT_EQ(error_code, ErrorCodes::TimedOut);

    // TODO
    //ASSERT_THROW(result.ThrowIfError(), std::system_error);
  }

  SIMPLE_TEST(Ignore) {
    // No warnings here!

    MakeVector(7).Ignore("Test");
    MakeError().Ignore("Test");
    Ok().Ignore("Test");
  }

  SIMPLE_TEST(MatchErrorCode) {
    Status result = Fail(TimedOut());
    ASSERT_TRUE(result.MatchErrorCode(ErrorCodes::TimedOut));
  }

  SIMPLE_TEST(Move) {
    auto result_1 = fallible::Ok<TestClass>({"Hello"});
    auto result_2 = std::move(result_1);

    ASSERT_EQ(result_2->Message(), "Hello");
    ASSERT_EQ(result_1->Message(), "");
  }

  SIMPLE_TEST(Copy) {
    auto result_1 = fallible::Ok<TestClass>({"Hello"});
    Result<TestClass> result_2 = result_1;

    ASSERT_EQ(result_1->Message(), "Hello");
    ASSERT_EQ(result_2->Message(), "Hello");
  }

  SIMPLE_TEST(AccessMethods) {
    auto result = fallible::Ok<TestClass>({"Hello"});
    ASSERT_EQ(result->Message(), "Hello");
    
    const TestClass& test = *result;
    ASSERT_EQ(test.Message(), "Hello");
    
    TestClass thief = std::move(*result);
    ASSERT_EQ(thief.Message(), "Hello");
    
    ASSERT_EQ(result.ValueOrThrow().Message(), "");
  }

  SIMPLE_TEST(Status) {
    {
      // Ok
      auto status = Ok();
      ASSERT_FALSE(status.Failed());
      ASSERT_TRUE(status.IsOk());
      status.ThrowIfError();  // Nothing happens
    }

    {
      // Fail
      Status err = Fail(TimedOut());
      ASSERT_TRUE(err.Failed());
      //    ASSERT_THROW(err_result.ThrowIfError(), std::system_error);

      ASSERT_EQ(err.ErrorCode(), ErrorCodes::TimedOut);
    }
  }

  SIMPLE_TEST(ExpectOk) {
    {
      Result<std::vector<int>> result = MakeVector(3);
      result.ExpectOk();
      result.ExpectOk("=(");
    }

    {
      Status status = Ok();
      status.ExpectOk();
      status.ExpectOk("=(");
    }

    {
      Result<std::vector<int>> result = MakeVector(3);
      auto vector = std::move(result.ExpectValue());
      ASSERT_EQ(vector.size(), 3);
    }

    {
      auto result = MakeError();
      //result.ExpectOk("=(");
    }
  }

  SIMPLE_TEST(MakeOkResult) {
    auto ok = fallible::Ok();
    ASSERT_TRUE(ok.IsOk());

    const size_t answer = 4;
    Result<size_t> result = fallible::Ok(answer);
  }

  SIMPLE_TEST(MakeErrorResult) {
    Result<std::string> response = fallible::Fail(TimedOut());
    ASSERT_FALSE(response.IsOk());

    Result<std::vector<std::string>> lines = fallible::PropagateError(response);
    ASSERT_FALSE(lines.IsOk());
  }

  Result<int> IntResult(int value) {
    return fallible::Ok(value);
  }

  /*
  SIMPLE_TEST(Consistency) {
    auto result = IntResult(0);
    if (!result) {
      FAIL_TEST("???");
    }

    if (!IntResult(0)) {
      FAIL();
    }
  }
  */

//  SIMPLE_TEST(Exceptions) {
//    auto bad = []() -> Result<std::string> {
//      try {
//        throw std::runtime_error("Bad");
//        return fallible::Ok<std::string>("Good");
//      } catch (...) {
//        return fallible::CurrentException();
//      }
//    };
//
//    auto result = bad();
//
//    ASSERT_TRUE(result.HasError());
//
//    auto error = result.GetError();
//    ASSERT_TRUE(error.HasException());
//    ASSERT_FALSE(error.HasErrorCode());
//
//    ASSERT_THROW(result.ThrowIfError(), std::runtime_error);
//  }



//  SIMPLE_TEST(Throw) {
//    Result<int> result = fallible::Throw<std::runtime_error>("Test error");
//
//    ASSERT_TRUE(result.HasError());
//    ASSERT_THROW(result.ThrowIfError(), std::runtime_error);
//  }

  struct MoveOnly {
    MoveOnly(int d) : data(d) {
    }

    MoveOnly(MoveOnly&& that) = default;
    MoveOnly& operator=(MoveOnly&& that) = default;

    MoveOnly(const MoveOnly& that) = delete;
    MoveOnly& operator=(const MoveOnly& that) = delete;

    int data;
  };

  SIMPLE_TEST(MoveOnly) {
    {
      auto r = Result<MoveOnly>::Ok(42);
      auto v = std::move(r).ValueOrThrow();
      ASSERT_EQ(v.data, 42);
    }

    {
      MoveOnly v = Result<MoveOnly>::Ok(17).ExpectValueOr("Fail");
      ASSERT_EQ(v.data, 17);
    }
  }

  SIMPLE_TEST(ConstResultValue) {
    const Result<int> result = fallible::Ok(42);
    ASSERT_EQ(result.ValueOrThrow(), 42);
  }

  SIMPLE_TEST(NotSupported) {
    Result<int> result = fallible::NotSupported();

    ASSERT_TRUE(result.Failed());
    ASSERT_TRUE(result.MatchErrorCode(ErrorCodes::NotSupported));
  }

  SIMPLE_TEST(Map) {
    {
      // Value
      auto result = Ok(7).Map([](int value) { return value + 1; });

      ASSERT_TRUE(result.IsOk());
      ASSERT_EQ(*result, 8);
    }

    {
      // Faulty
      auto result = Ok(7).Map([](int /*value*/) {
        return Fail(TimedOut()).As<int>();
      });

      ASSERT_FALSE(result.IsOk());
    }

    {
      // Result
      auto result = Ok(7).Map([](Result<int> input) -> Result<int> {
        if (input.IsOk()) {
          return Ok(*input + 1);
        } else {
          return PropagateError(input);
        }
      });

      ASSERT_TRUE(result.IsOk());
      ASSERT_EQ(*result, 8);
    }

    {
      // Recover
      auto result = Fail(TimedOut()).As<int>().Map([](int value) {
        std::abort();
        return value + 1;
      }).Recover([](Error) {
        return Ok(7);
      }).Map([](int value) {
        return value + 1;
      });

      ASSERT_TRUE(result.IsOk());
      ASSERT_EQ(*result, 8);
    }

    {
      // VoidMapper

      Result<int> result = Ok().Map([]() -> int {
        std::cout << "Hi" << std::endl;
        return 7;
      });

      ASSERT_TRUE(result.IsOk());
      ASSERT_EQ(*result, 7);
    }

    {
      // Worker

      Result<wheels::Unit> result = Ok().Map([]() {
        std::cout << "Worker" << std::endl;
      });

      ASSERT_TRUE(result.IsOk());
    }
  }

  struct Cancelled : fallible::IgnoreThisException {};

  SIMPLE_TEST(ThrowingMap) {
    {
      auto result = Ok(7).Map([](int) -> int {
        throw std::runtime_error("Failed to increment");
      });

      ASSERT_TRUE(result.Failed());
      auto error = result.Error();
      std::cout << error.Reason() << std::endl;
    }

    {
      bool cancelled = false;

      try {
        auto result = Ok(7).Map([](int) -> int { throw Cancelled{}; });
      } catch (Cancelled&) {
         cancelled = true;
      }

      ASSERT_TRUE(cancelled);
    }
  }

  SIMPLE_TEST(JustStatus) {
    {
      auto status = Ok(7).JustStatus();
      ASSERT_TRUE(status.IsOk());
    }

    {
      auto status = Fail(TimedOut()).As<int>().JustStatus();
      ASSERT_FALSE(status.IsOk());
    }
  }

  SIMPLE_TEST(MapResult) {
    using Input = int;

    auto mapper = [](Input input) {
      return input;
    };

    using MapResult = fallible::MapResult<Input, decltype(mapper)>;
    using MapValue = typename MapResult::ValueType;

    bool same = std::is_same_v<int, MapValue>;
    ASSERT_TRUE(same);
  }

  SIMPLE_TEST(EatValue) {
    auto status = fallible::Ok(7).Map([](int value) {
      std::cout << value << std::endl;
    });

    ASSERT_TRUE(status.IsOk());
  }

  SIMPLE_TEST(EatResult) {
    auto status = Fail(TimedOut()).As<int>().Map([](Result<int> result) {
      ASSERT_TRUE(result.Failed());
    });

    ASSERT_TRUE(status.IsOk());
  }
}

