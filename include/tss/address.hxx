#pragma once

#include "enums.hxx"

#include <cstdint>
#include <string_view>
#include <tuple>

namespace tss {
  namespace detail {
    template<ip_version_t TIP>
    struct ip_address;

    template<>
    struct ip_address<ip_version_t::V4> final {
      using type = std::tuple<std::uint8_t, std::uint8_t, std::uint8_t, std::uint8_t>;
    };

    template<>
    struct ip_address<ip_version_t::V6> final {
      using type = std::tuple<
          std::uint16_t, std::uint16_t, std::uint16_t, std::uint16_t,
          std::uint16_t, std::uint16_t, std::uint16_t, std::uint16_t
      >;
    };
  }

  template<ip_version_t TIP>
  using ip_address_t = typename detail::ip_address<TIP>::type;

  using ip_address_v4_t = ip_address_t<ip_version_t::V4>;
  using ip_address_v6_t = ip_address_t<ip_version_t::V6>;

  using port_t = std::uint16_t;

  template<ip_version_t TIP>
  using address_t = std::tuple<ip_address_t<TIP>, port_t>;

  using address_v4_t = address_t<ip_version_t::V4>;
  using address_v6_t = address_t<ip_version_t::V6>;

  ip_address_v4_t resolve_ip_address_v4(std::string_view address);
  ip_address_v6_t resolve_ip_address_v6(std::string_view address);
}
