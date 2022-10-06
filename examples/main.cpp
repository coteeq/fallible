#include <wheels/result/result.hpp>
#include <wheels/result/make.hpp>

using wheels::ErrorCodes;

// Error = code [+ domain] [+ reason] [+ location] [+ context]
using wheels::Error;
// Result<T> = Error | value of type T
using wheels::Result;

// Error constructor
using wheels::Err;

// Result constructors
using wheels::result::Ok;
using wheels::result::PropagateError;
using wheels::result::Fail;

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
      auto error = result.GetError();
      // Or use error.GetCode(), error.GetReason() etc
      std::cout << "Bar() -> " << error.AsJson().dump(1, ' ') << std::endl;
    }
  }

  // #5: Panic on failure

  Bar().ExpectOk("Cannot survive Bar failure");  // Process crashed

  return 0;
}
