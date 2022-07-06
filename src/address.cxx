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

namespace tss {
  ip_address_v4_t resolve_ip_address_v4(std::string_view const address, native::socket_api const&)
  {
    addrinfo hints{};
    hints.ai_family = AF_INET;

    addrinfo* infos{nullptr};

    if (auto const result = ::getaddrinfo(address.data(), nullptr, &hints, &infos); result!=0) {
      throw address_info_error{result};
    }

    auto const sa = reinterpret_cast<sockaddr_in const*>(infos->ai_addr);
    std::uint32_t const full{ntohl(sa->sin_addr.s_addr)};

    ip_address_v4_t addr{
        static_cast<std::uint8_t>(full >> 24U),
        static_cast<std::uint8_t>(full >> 16U),
        static_cast<std::uint8_t>(full >> 8U),
        static_cast<std::uint8_t>(full),
    };

    ::freeaddrinfo(infos);

    return addr;
  }

  ip_address_v6_t resolve_ip_address_v6(std::string_view address, native::socket_api const&)
  {
    addrinfo hints{};
    hints.ai_family = AF_INET6;

    addrinfo* infos{nullptr};

    if (auto const result = ::getaddrinfo(address.data(), nullptr, &hints, &infos); result!=0) {
      throw address_info_error{result};
    }

    auto const sa = reinterpret_cast<sockaddr_in6 const*>(infos->ai_addr);

#if defined(_WIN32)
#define W u.Word
#else
#define W __u6_addr.__u6_addr16
#endif

    ip_address_v6_t addr{
        ntohs(sa->sin6_addr.W[0]),
        ntohs(sa->sin6_addr.W[1]),
        ntohs(sa->sin6_addr.W[2]),
        ntohs(sa->sin6_addr.W[3]),
        ntohs(sa->sin6_addr.W[4]),
        ntohs(sa->sin6_addr.W[5]),
        ntohs(sa->sin6_addr.W[6]),
        ntohs(sa->sin6_addr.W[7]),
    };

#undef W

    ::freeaddrinfo(infos);

    return addr;
  }
}
