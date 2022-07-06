#include <tss/address.hxx>
#include <tss/exceptions.hxx>

#if defined(_WIN32)
#error TODO: implement windows version
#else

#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#endif

namespace tss {
  ip_address_v4_t resolve_ip_address_v4(std::string_view const address)
  {
    addrinfo hints{};
    hints.ai_family = AF_INET;

    addrinfo* infos{nullptr};

    if (auto const result = ::getaddrinfo(address.data(), nullptr, &hints, &infos); result!=0) {
      throw address_info_error{result};
    }

    auto const sa = reinterpret_cast<sockaddr_in const*>(infos->ai_addr);
    std::uint32_t const full{ntohl(sa->sin_addr.s_addr)};

    ip_address_v4_t const addr{
        static_cast<std::uint8_t>(full >> 24U),
        static_cast<std::uint8_t>(full >> 16U),
        static_cast<std::uint8_t>(full >> 8U),
        static_cast<std::uint8_t>(full),
    };

    ::freeaddrinfo(infos);

    return addr;
  }

  ip_address_v6_t resolve_ip_address_v6(std::string_view address)
  {
    addrinfo hints{};
    hints.ai_family = AF_INET6;

    addrinfo* infos{nullptr};

    if (auto const result = ::getaddrinfo(address.data(), nullptr, &hints, &infos); result!=0) {
      throw address_info_error{result};
    }

    auto const sa = reinterpret_cast<sockaddr_in6 const*>(infos->ai_addr);

    ip_address_v6_t const addr{
        ntohs(sa->sin6_addr.__u6_addr.__u6_addr16[0]),
        ntohs(sa->sin6_addr.__u6_addr.__u6_addr16[1]),
        ntohs(sa->sin6_addr.__u6_addr.__u6_addr16[2]),
        ntohs(sa->sin6_addr.__u6_addr.__u6_addr16[3]),
        ntohs(sa->sin6_addr.__u6_addr.__u6_addr16[4]),
        ntohs(sa->sin6_addr.__u6_addr.__u6_addr16[5]),
        ntohs(sa->sin6_addr.__u6_addr.__u6_addr16[6]),
        ntohs(sa->sin6_addr.__u6_addr.__u6_addr16[7]),
    };

    ::freeaddrinfo(infos);

    return addr;
  }
}
