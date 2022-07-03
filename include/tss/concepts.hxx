#pragma once

#include <concepts>
#include <type_traits>

#include "native.hxx"

namespace tss::concepts {
  template<typename T>
  concept Data = std::is_standard_layout_v<T>;

  template<typename T>
  concept Socket = requires(T const* t) {
    { t->native_handle() } -> std::same_as<native::socket_traits::socket_t>;
  };
}
