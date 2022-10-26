#include <fallible/result/result.hpp>
#include <fallible/result/make.hpp>

using fallible::ErrorCodes;

// Error = code [+ domain] [+ reason] [+ location] [+ context]
using fallible::Error;
// Result<T> = Error | value of type T
using fallible::Result;

// Error constructor
using fallible::Err;

// Result constructors
using fallible::Ok;
using fallible::PropagateError;
using fallible::Fail;


template <fallible::FaultyMapper<int> Mapper>
auto Map(Result<int> input, Mapper mapper) {
  return input.Map(std::move(mapper));
}

//////////////////////////////////////////////////////////////////////

Result<int> Foo() {
  return Ok(42);
}

Result<std::string> Bar() {
  return Fail(
      Err(ErrorCodes::Unknown)
          .Domain("Canonical")
          .Reason("Something went wrong")
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

  Bar();  // Compiler warning (or error with -werror flag): Result ignored

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
      std::cout << "Bar() -> " << error.AsJson().dump(1, ' ') << std::endl;
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
      // Recover from error

      auto result = Bar()
                        .Map([](std::string /*input*/) -> std::string {
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
