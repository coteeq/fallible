#include <fallible/result/result.hpp>
#include <fallible/result/make.hpp>

#include <cassert>

//////////////////////////////////////////////////////////////////////

using fallible::ErrorCodes;

// Context = domain + reason + source + attrs
using fallible::Context;
// Error = code + Context
using fallible::Error;
// Result<T> = Error | value of type T
using fallible::Result;

// Context constructor
using fallible::Ctx;
// Error constructor
using fallible::Err;
// Result constructors
using fallible::Ok;
using fallible::PropagateError;
using fallible::Fail;

//////////////////////////////////////////////////////////////////////

Result<int> Foo() {
  return Ok(42);  // Wrap value to Result
}

Result<std::string> Bar() {
  return Fail(
      Err(ErrorCodes::Unknown)
          .Domain("Canonical")
          .Reason("Something went wrong")
          .Attr("key1", "value1")
          .Attr("key2", "value2")
          .Done());
}

Result<int> Baz() {
  auto result = Bar();
  if (result.IsOk()) {
    return Ok(7);
  } else {
    // Automagically adapts to different value type
    return PropagateError(result);
  }
}

//////////////////////////////////////////////////////////////////////

int main() {
  // #1: Unwrap result or panic (abort process)

  std::cout << "Foo() -> "
            << Foo().ExpectValueOr("Fail miserably")  // Success
            << std::endl;

  // #2: Ignore result

  Bar().Ignore();  // Compiler warning (or error with -werror flag): Result ignored

  // #3: Intentionally ignore result

  Bar().Ignore();  // Intentionally suppress compiler warning

  // #4: Check for success, then unwrap

  {
    auto result = Bar();

    if (result.IsOk()) {
      // Access value via * operator
      // Behavior is undefined if result holds an error instead of a value
      std::cout << "Bar() -> " << *result << std::endl;
    } else {
      auto error = result.Error();
      // Or use error.GetCode(), error.GetReason() etc
      std::cout << "Bar() -> " << error.Describe() << std::endl;
    }
  }

  // #5: Monadic API
  {
    {
      auto result = Foo()
                        .Map([](int value) -> int {
                          return value + 1;
                        })
                        .Map([](Result<int> input) {
                          return Ok(*input + 1);
                        });

      std::cout << "Foo.Map.Map -> " << *result << std::endl;
    }

    {
      // Wrap exceptions to errors

      auto result = Foo()
                        .Map([](int /*value*/) -> int {
                          throw std::runtime_error("Failed");
                        }).Map([](int value) {
                          // Skipped
                          return value + 1;
                        });

      assert(result.Failed());
      std::cout << "Foo.Throw.Map -> Error" << std::endl;
    }

    {
      // Recover from error

      auto result = Bar()
                        .Map([](std::string /*input*/) -> std::string {
                          // Skipped
                          std::abort();
                        }).Recover([](Error /*error*/) {
                          return Ok<std::string>("Hello");
                        });

      assert(result.IsOk());
      std::cout << "Bar.Map.Recover -> " << *result << std::endl;
    }
  }

  // #6: Panic on failure

  Bar().ExpectOk("Cannot survive Bar failure");  // Process crashed

  return 0;
}
