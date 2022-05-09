#pragma once
#include <type_traits>

namespace utility {
template <typename E>
constexpr typename std::underlying_type<E>::type etoi(E e) noexcept {
    return static_cast<typename std::underlying_type<E>::type>(e);
}
}  // namespace utility