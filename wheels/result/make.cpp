#include <wheels/result/make.hpp>

#include <wheels/result/error_codes.hpp>

namespace wheels {

namespace make_result {

Status Ok() {
  return Status::Ok();
}

detail::Failure Fail(Error error) {
  return detail::Failure(std::move(error));
}

Status ToStatus(std::error_code error) {
  // TODO: match error codes
  if (error) {
    return Fail(Err(error.value())
                    .Domain(error.category().name())
                    .Reason(error.message())
                    .Done());
  } else {
    return Ok();
  }
}

detail::Failure NotSupported() {
  return Fail(Err(ErrorCodes::NotSupported)
                  .Domain("Canonical")
                  .Reason("Not supported")
                  .Done());
}

}  // namespace make_result

}  // namespace wheels
