#pragma once

namespace fallible {

// Do not wrap exceptions inherited from IgnoreThisException into Error
// in Result::{Map,Recover}

struct IgnoreThisException {
  //
};

}  // namespace fallible
