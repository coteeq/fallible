#include <wheels/result/result.hpp>
#include <wheels/result/make.hpp>

#include <wheels/test/test_framework.hpp>

using wheels::Result;
using wheels::Status;
namespace make_result = wheels::make_result;

////////////////////////////////////////////////////////////////////////////////

static std::error_code TimedOut() {
  return std::make_error_code(std::errc::timed_out);
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
  return make_result::Ok(std::move(ints));
}

Result<std::string> MakeError() {
  return make_result::Fail(TimedOut());
}

wheels::Status MakeOkStatus() {
  return wheels::Status::Ok();
}

////////////////////////////////////////////////////////////////////////////////


TEST_SUITE(Result) {

  SIMPLE_TEST(Ok) {
    static const std::string kMessage = "Hello";

    auto result = Result<TestClass>::Ok(kMessage);
    ASSERT_TRUE(result.IsOk());
    ASSERT_FALSE(result.HasError());
    result.ThrowIfError();  // Nothing happens

    ASSERT_EQ(result.ValueUnsafe().Message(), kMessage);
    ASSERT_EQ((*result).Message(), "Hello");

    ASSERT_EQ(result->Message(), kMessage);
  }

  SIMPLE_TEST(ObjectCount) {
    {
      auto result = Result<TestClass>::Ok("Hi");
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
    ASSERT_TRUE(result.HasError());

    auto error = result.GetErrorCode();
    ASSERT_EQ(error.value(), (int)std::errc::timed_out);

    ASSERT_THROW(result.ThrowIfError(), std::system_error);
  }

  SIMPLE_TEST(Ignore) {
    // No warnings here!

    MakeVector(7).Ignore();
    MakeError().Ignore();
    MakeOkStatus().Ignore();
  }

  SIMPLE_TEST(MatchErrorCode) {
    Result<void> result = make_result::Fail(TimedOut());
    ASSERT_TRUE(result.MatchErrorCode((int)std::errc::timed_out));
  }

  SIMPLE_TEST(Move) {
    auto result_1 = Result<TestClass>::Ok("Hello");
    auto result_2 = std::move(result_1);
    ASSERT_EQ(result_2.ValueOrThrow().Message(), "Hello");
  }

  SIMPLE_TEST(Copy) {
    auto result_1 = Result<TestClass>::Ok("Hello");
    Result<TestClass> result_2 = result_1;
    ASSERT_EQ(result_1.ValueOrThrow().Message(), "Hello");
  }

  SIMPLE_TEST(AccessMethods) {
    auto result = Result<TestClass>::Ok("Hello");
    ASSERT_EQ(result->Message(), "Hello");
    
    const TestClass& test = *result;
    ASSERT_EQ(test.Message(), "Hello");
    
    TestClass thief = std::move(*result);
    ASSERT_EQ(thief.Message(), "Hello");
    
    ASSERT_EQ(result.ValueOrThrow().Message(), "");
  }

  SIMPLE_TEST(Void) {
    // Ok
    auto result = Result<void>::Ok();
    ASSERT_FALSE(result.HasError());
    ASSERT_TRUE(result.IsOk());
    result.ThrowIfError();  // Nothing happens
    
    // Fail
    auto err_result = Result<void>::Fail(TimedOut());
    ASSERT_TRUE(err_result.HasError());
    ASSERT_THROW(err_result.ThrowIfError(), std::system_error);
    auto error = err_result.GetErrorCode();
    ASSERT_EQ(error.value(), (int)std::errc::timed_out);
  }

  SIMPLE_TEST(AutomaticallyUnwrapRvalue) {
    std::vector<int> ints = MakeVector(3);
    ASSERT_EQ(ints.size(), 3u);
    
    auto result_1 = MakeVector(4);
    ints = std::move(result_1);
    ASSERT_EQ(ints.size(), 4u);
    
    auto result_2 = MakeVector(5);
    // Does not compiled
    // ints = result_2;
    
    std::string str;
    ASSERT_THROW(str = MakeError(), std::system_error);
  }

  SIMPLE_TEST(ExpectOk) {
    {
      Result<std::vector<int>> result = MakeVector(3);
      result.ExpectOk();
      result.ExpectOk("=(");
    }

    {
      Status status = MakeOkStatus();
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
    auto ok = make_result::Ok();
    ASSERT_TRUE(ok.IsOk());

    const size_t answer = 4;
    Result<size_t> result = make_result::Ok(answer);
  }

  SIMPLE_TEST(MakeErrorResult) {
    Result<std::string> response = make_result::Fail(TimedOut());
    ASSERT_FALSE(response.IsOk());

    Result<std::vector<std::string>> lines = make_result::PropagateError(response);
    ASSERT_FALSE(lines.IsOk());
  }

  SIMPLE_TEST(MakeResultStatus) {
    auto result = make_result::ToStatus(TimedOut());
    ASSERT_FALSE(result.IsOk());
    ASSERT_TRUE(result.HasError());
  }

  Result<int> IntResult(int value) {
    return make_result::Ok(value);
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

  SIMPLE_TEST(JustStatus) {
    auto answer = Result<int>::Ok(42);
    auto ok = make_result::JustStatus(answer);
    ASSERT_TRUE(ok.IsOk());

    auto response = Result<std::string>::Fail(TimedOut());
    auto fail = make_result::JustStatus(response);
    ASSERT_FALSE(fail.IsOk());
  }

  SIMPLE_TEST(Exceptions) {
    auto bad = []() -> Result<std::string> {
      try {
        throw std::runtime_error("Bad");
        return make_result::Ok<std::string>("Good");
      } catch (...) {
        return make_result::CurrentException();
      }
    };

    auto result = bad();

    ASSERT_TRUE(result.HasError());

    auto error = result.GetError();
    ASSERT_TRUE(error.HasException());
    ASSERT_FALSE(error.HasErrorCode());

    ASSERT_THROW(result.ThrowIfError(), std::runtime_error);
  }

  SIMPLE_TEST(Invoke) {
    {
      auto good = []() -> int {
        return 42;
      };

      auto result = make_result::Invoke(good);
      ASSERT_TRUE(result.HasValue());
      ASSERT_EQ(*result, 42);
    }

    {
      auto bad = []() -> int {
        throw std::runtime_error("just test");
      };

      auto result = make_result::Invoke(bad);
      ASSERT_TRUE(result.HasError());
    }
  }

  SIMPLE_TEST(InvokeWithArguments) {
    auto sum = [](int x, int y) { return x + y; };
    auto result = make_result::Invoke(sum, 1, 2);
    ASSERT_EQ(*result, 3);
  }

  SIMPLE_TEST(InvokeVoid) {
    bool done = false;
    auto work = [&]() {
      done = true;
    };
    Status status = make_result::Invoke(work);
    ASSERT_TRUE(status.IsOk());
    ASSERT_TRUE(done);
  }

  SIMPLE_TEST(Throw) {
    Result<int> result = make_result::Throw<std::runtime_error>("Test error");

    ASSERT_TRUE(result.HasError());
    ASSERT_THROW(result.ThrowIfError(), std::runtime_error);
  }

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

  SIMPLE_TEST(InvokeMoveOnlyArguments) {
    auto foo = [](MoveOnly mo) {
      return MoveOnly(mo.data + 1);
    };

    auto result = make_result::Invoke(foo, MoveOnly(3));
    ASSERT_EQ(result->data, 4);
  }

  SIMPLE_TEST(ConstResultValue) {
    const Result<int> result = make_result::Ok(42);
    ASSERT_EQ(result.ValueOrThrow(), 42);
  }

  SIMPLE_TEST(NotSupported) {
    Result<int> result = make_result::NotSupported();

    ASSERT_TRUE(result.HasError());
    ASSERT_TRUE(result.MatchErrorCode((int)std::errc::not_supported));
  }
}

