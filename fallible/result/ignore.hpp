#pragma once

namespace fallible {

// Do not wrap exception tagged with IgnoreThisException into Error
// in Result::{Map,Recover}

struct IgnoreThisException {};

}  // namespace fallible
