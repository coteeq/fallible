#pragma once

#include <fallible/result/mappers.hpp>

namespace fallible {

template <typename Input, typename Mapper>
using MapResult = decltype(std::declval<Result<Input>>().Map(std::declval<Mapper>()));

}  // namespace fallible
