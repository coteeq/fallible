#include <wheels/result/result.hpp>
#include <wheels/result/make.hpp>

using wheels::ErrorCodes;

// Error = code + domain [+ reason] [+ location] [+ context]
using wheels::Error;
// Result<T> = Error | value of type T
using wheels::Result;

// Error constructor
using wheels::Err;

// Result constructors
using wheels::make_result::Ok;
using wheels::make_result::PropagateError;
using wheels::make_result::Fail;

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
  std::cout << "Foo() -> "
            << Foo().ExpectValueOr("Fail miserably")  // Success
            << std::endl;

  Bar();  // Compiler warning (or error with -werror flag): Result ignored

  Bar().Ignore();  // Intentionally suppress compiler warning

  // #4
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

  Bar().ExpectOk("Cannot survive Bar failure");  // Process crashed

  return 0;
}
