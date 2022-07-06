#include <tss/address.hxx>
#include <tss/exceptions.hxx>

#if defined(_WIN32)

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#else

#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#endif

namespace {

#if defined(_WIN32)
#define W u.Word
#else
#define W __u6_addr.__u6_addr16
#endif

  tss::ip_address_v6_t make_ip_address(in6_addr const& addr) noexcept
  {
    return {
        ntohs(addr.W[0]),
        ntohs(addr.W[1]),
        ntohs(addr.W[2]),
        ntohs(addr.W[3]),
        ntohs(addr.W[4]),
        ntohs(addr.W[5]),
        ntohs(addr.W[6]),
        ntohs(addr.W[7]),
    };
  }

#undef W

  tss::ip_address_v4_t make_ip_address(in_addr const& addr) noexcept
  {
    std::uint32_t const full{ntohl(addr.s_addr)};

    return {
        static_cast<std::uint8_t>(full >> 24U),
        static_cast<std::uint8_t>(full >> 16U),
        static_cast<std::uint8_t>(full >> 8U),
        static_cast<std::uint8_t>(full),
    };
  }
}

namespace tss {
  ip_address_v4_t resolve_ip_address_v4(std::string_view const address, native::socket_api const&)
  {
    addrinfo hints{};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    addrinfo* infos{nullptr};

    if (auto const result = ::getaddrinfo(address.data(), nullptr, &hints, &infos); result!=0) {
      throw address_info_error{result};
    }

    auto const sa = reinterpret_cast<sockaddr_in const*>(infos->ai_addr);
    auto const addr = ::make_ip_address(sa->sin_addr);

    ::freeaddrinfo(infos);

    return addr;
  }

  ip_address_v6_t resolve_ip_address_v6(std::string_view address, native::socket_api const&)
  {
    addrinfo hints{};
    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_STREAM;

    addrinfo* infos{nullptr};

    if (auto const result = ::getaddrinfo(address.data(), nullptr, &hints, &infos); result!=0) {
      throw address_info_error{result};
    }

    auto const sa = reinterpret_cast<sockaddr_in6 const*>(infos->ai_addr);
    auto const addr = ::make_ip_address(sa->sin6_addr);

    ::freeaddrinfo(infos);

    return addr;
  }

  std::vector<ip_address_v4_t> resolve_ip_addresses_v4(std::string_view address, native::socket_api const&)
  {
    addrinfo hints{};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    addrinfo* infos{nullptr};

    if (auto const result = ::getaddrinfo(address.data(), nullptr, &hints, &infos); result!=0) {
      throw address_info_error{result};
    }

    std::vector<ip_address_v4_t> addresses{};
    for (auto current_info = infos; current_info!=nullptr; current_info = current_info->ai_next) {
      auto const sa = reinterpret_cast<sockaddr_in const*>(current_info->ai_addr);
      addresses.push_back(::make_ip_address(sa->sin_addr));
    }

    ::freeaddrinfo(infos);

    return addresses;
  }

  std::vector<ip_address_v6_t> resolve_ip_addresses_v6(std::string_view address, native::socket_api const&)
  {
    addrinfo hints{};
    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_STREAM;

    addrinfo* infos{nullptr};

    if (auto const result = ::getaddrinfo(address.data(), nullptr, &hints, &infos); result!=0) {
      throw address_info_error{result};
    }

    std::vector<ip_address_v6_t> addresses{};
    for (auto current_info = infos; current_info!=nullptr; current_info = current_info->ai_next) {
      auto const sa = reinterpret_cast<sockaddr_in6 const*>(current_info->ai_addr);
      addresses.push_back(::make_ip_address(sa->sin6_addr));
    }

    ::freeaddrinfo(infos);

    return addresses;
  }

  std::vector<std::variant<ip_address_v4_t, ip_address_v6_t>>
  resolve_ip_addresses(std::string_view address, native::socket_api const&)
  {
    addrinfo hints{};
    hints.ai_socktype = SOCK_STREAM;

    addrinfo* infos{nullptr};

    if (auto const result = ::getaddrinfo(address.data(), nullptr, &hints, &infos); result!=0) {
      throw address_info_error{result};
    }

    std::vector<std::variant<ip_address_v4_t, ip_address_v6_t>> addresses{};
    for (auto current_info = infos; current_info!=nullptr; current_info = current_info->ai_next) {
      if (current_info->ai_family==AF_INET) {
        auto const sa = reinterpret_cast<sockaddr_in const*>(current_info->ai_addr);
        addresses.emplace_back(::make_ip_address(sa->sin_addr));
      }
      else if (current_info->ai_family==AF_INET6) {
        auto const sa = reinterpret_cast<sockaddr_in6 const*>(current_info->ai_addr);
        addresses.emplace_back(::make_ip_address(sa->sin6_addr));
      }
    }

    ::freeaddrinfo(infos);

    return addresses;
  }
}
