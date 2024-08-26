#include <tss/socket.hxx>
#include <tss/exceptions.hxx>

#include <limits>

#if defined(_WIN32)

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <Windows.h>
#include <WinSock2.h>
#include <ws2ipdef.h>

#else

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#endif

#include <gsl/assert>

namespace {
  template<tss::ip_version_t TIP>
  inline auto constexpr af = AF_INET;

  template<>
  inline auto constexpr af<tss::ip_version_t::V6> = AF_INET6;

  template<tss::ip_version_t TIP>
  using sockaddr_t = std::conditional_t<TIP==tss::ip_version_t::V6, sockaddr_in6, sockaddr_in>;

  template<tss::protocol_t TProto>
  inline auto constexpr type = SOCK_STREAM;

  template<>
  inline auto constexpr type<tss::protocol_t::UDP> = SOCK_DGRAM;

  template<tss::protocol_t TProto>
  inline auto constexpr proto = IPPROTO_TCP;

  template<>
  inline auto constexpr proto<tss::protocol_t::UDP> = IPPROTO_UDP;

#if defined(_WIN32)
#define W u.Word
#else
#define W s6_addr16
#endif

  in6_addr make_in_addr(tss::ip_address_v6_t const& ip) noexcept
  {
    in6_addr addr{};
    addr.W[0] = htons(std::get<0U>(ip));
    addr.W[1] = htons(std::get<1U>(ip));
    addr.W[2] = htons(std::get<2U>(ip));
    addr.W[3] = htons(std::get<3U>(ip));
    addr.W[4] = htons(std::get<4U>(ip));
    addr.W[5] = htons(std::get<5U>(ip));
    addr.W[6] = htons(std::get<6U>(ip));
    addr.W[7] = htons(std::get<7U>(ip));
    return addr;
  }

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

  sockaddr_in6 make_sock_addr(tss::address_v6_t const& addr) noexcept
  {
    auto const [ip, port] = addr;

    sockaddr_in6 result{};
    result.sin6_family = AF_INET6;
    result.sin6_addr = make_in_addr(ip);
    result.sin6_port = htons(port);
    return result;
  }

  tss::address_v6_t make_address(sockaddr_in6 const& addr) noexcept
  {
    return {
        make_ip_address(addr.sin6_addr),
        ntohs(addr.sin6_port),
    };
  }

  in_addr make_in_addr(tss::ip_address_v4_t const& ip) noexcept
  {
    std::uint32_t const full{
        (static_cast<std::uint32_t>(std::get<0U>(ip)) << 24U) |
            (static_cast<std::uint32_t>(std::get<1U>(ip)) << 16U) |
            (static_cast<std::uint32_t>(std::get<2U>(ip)) << 8U) |
            static_cast<std::uint32_t>(std::get<3U>(ip))
    };

    in_addr addr{};
    addr.s_addr = htonl(full);
    return addr;
  }

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

  sockaddr_in make_sock_addr(tss::address_v4_t const& addr) noexcept
  {
    auto const [ip, port] = addr;

    sockaddr_in result{};
    result.sin_family = AF_INET;
    result.sin_addr = make_in_addr(ip);
    result.sin_port = htons(port);
    return result;
  }

  tss::address_v4_t make_address(sockaddr_in const& addr) noexcept
  {
    return {
        make_ip_address(addr.sin_addr),
        ntohs(addr.sin_port),
    };
  }
}

namespace tss {
  namespace detail {
    template<ip_version_t TIP, protocol_t TProto>
    socket_base<TIP, TProto>::socket_base(native::socket_api const&)
        : handle_{::socket(::af<TIP>, ::type<TProto>, ::proto<TProto>)}
    {
      if (handle_==traits::invalid_value) {
        throw socket_error{};
      }
    }

    template<ip_version_t TIP, protocol_t TProto>
    socket_base<TIP, TProto>::socket_base(socket_base&& src) noexcept
        : handle_{src.handle_}
    {
      src.handle_ = traits::invalid_value;
    }

    template<ip_version_t TIP, protocol_t TProto>
    socket_base<TIP, TProto>::~socket_base() noexcept
    {
      try {
        close();
      }
      catch (tss::socket_error const& ex) {
        (void) ex;
      }
    }

    template<ip_version_t TIP, protocol_t TProto>
    socket_base<TIP, TProto>::traits::socket_t socket_base<TIP, TProto>::native_handle() const noexcept
    {
      return handle_;
    }

    template<ip_version_t TIP, protocol_t TProto>
    void socket_base<TIP, TProto>::close()
    {
      if (handle_!=traits::invalid_value) {
#if defined(_WIN32)
        ::closesocket(handle_);
#else
        ::close(handle_);
#endif
      }
      handle_ = traits::invalid_value;
    }

    template<ip_version_t TIP, protocol_t TProto>
    bool socket_base<TIP, TProto>::is_valid() const noexcept
    {
      return handle_!=traits::invalid_value;
    }

    template<ip_version_t TIP, protocol_t TProto>
    void socket_base<TIP, TProto>::bind(address_t<TIP> const& address)
    {
      auto const addr = make_sock_addr(address);
      auto const result = ::bind(handle_, reinterpret_cast<sockaddr const*>(&addr), traits::socklen_t{sizeof(addr)});
      if (result==-1) {
        throw socket_error{};
      }
    }

    template<ip_version_t TIP, protocol_t TProto>
    void socket_base<TIP, TProto>::set_reuse_addr(bool const reuse)
    {
      int value{reuse ? 1 : 0};
      auto const result = ::setsockopt(handle_, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<traits::send_buf_t>(&value),
          static_cast<traits::socklen_t >(sizeof(value)));
      if (result==-1) {
        throw socket_error{};
      }
    }

    template<ip_version_t TIP, protocol_t TProto>
    bool socket_base<TIP, TProto>::get_reuse_addr() const
    {
      int reuse{};
      auto len{static_cast<traits::socklen_t >(sizeof(reuse))};
      auto const result = ::getsockopt(handle_, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<traits::recv_buf_t>(&reuse),
          &len);
      if (result==-1) {
        throw socket_error{};
      }
      return !!reuse;
    }

    template<ip_version_t TIP, protocol_t TProto>
    socket_base<TIP, TProto>::socket_base(traits::socket_t const handle) noexcept
        : handle_{handle}
    {
    }

    template
    class socket_base<ip_version_t::V4, protocol_t::TCP>;

    template
    class socket_base<ip_version_t::V4, protocol_t::UDP>;

    template
    class socket_base<ip_version_t::V6, protocol_t::TCP>;

    template
    class socket_base<ip_version_t::V6, protocol_t::UDP>;
  }

  template<ip_version_t TIP>
  void socket<TIP, protocol_t::TCP>::listen(std::uint32_t const backlog)
  {
    static std::uint32_t constexpr backlog_mask = std::numeric_limits<int>::max();
    if (::listen(handle_, static_cast<int>(backlog & backlog_mask))==-1) {
      throw socket_error{};
    }
  }

  template<ip_version_t TIP>
  void socket<TIP, protocol_t::TCP>::connect(tss::address_t<TIP> const& address)
  {
    auto const addr = make_sock_addr(address);
    auto const result = ::connect(
        handle_,
        reinterpret_cast<sockaddr const*>(&addr),
        static_cast<traits::socklen_t>(sizeof(addr))
    );
    if (result==-1) {
      throw socket_error{};
    }
  }

  template<ip_version_t TIP>
  socket<TIP, protocol_t::TCP> socket<TIP, protocol_t::TCP>::accept(address_t<TIP>* address)
  {
    ::sockaddr_t<TIP> addr{};
    auto addr_len{static_cast<traits::socklen_t>(sizeof(addr))};
    auto const result = ::accept(
        handle_,
        reinterpret_cast<sockaddr*>(&addr),
        &addr_len
    );
    if (result==traits::invalid_value) {
      throw socket_error{};
    }
    return socket{result};
  }

  template<ip_version_t TIP>
  void socket<TIP, protocol_t::TCP>::shutdown(shutdown_t const how)
  {
    int native_how{};
#if defined(_WIN32)
    switch (how) {
    case shutdown_t::Read:
      native_how = SD_RECEIVE;
      break;
    case shutdown_t::Write:
      native_how = SD_SEND;
      break;
    case shutdown_t::ReadWrite:
      native_how = SD_BOTH;
      break;
    default:
      Expects(native_how!=0);
    }
#else
    switch (how) {
    case shutdown_t::Read:
      native_how = SHUT_RD;
      break;
    case shutdown_t::Write:
      native_how = SHUT_WR;
      break;
    case shutdown_t::ReadWrite:
      native_how = SHUT_RDWR;
      break;
    default:
      Expects(native_how!=0);
    }
#endif

    auto const result = ::shutdown(handle_, native_how);
    if (result==-1) {
      throw socket_error{};
    }
  }

  template<ip_version_t TIP>
  std::size_t socket<TIP, protocol_t::TCP>::send_(std::byte const* const data, std::size_t const data_length)
  {
    auto const result = ::send(
        handle_,
        reinterpret_cast<traits::send_buf_t>(data),
        static_cast<traits::buflen_t>(data_length),
        0
    );
    if (result==-1) {
      throw socket_error{};
    }
    return static_cast<std::size_t>(result);
  }

  template<ip_version_t TIP>
  std::size_t socket<TIP, protocol_t::TCP>::receive_(std::byte* const buffer, std::size_t const buffer_length)
  {
    auto const result = ::recv(
        handle_,
        reinterpret_cast<traits::recv_buf_t>(buffer),
        static_cast<traits::buflen_t>(buffer_length),
        0
    );
    if (result==-1) {
      throw socket_error{};
    }
    return static_cast<std::size_t>(result);
  }

  template<ip_version_t TIP>
  std::size_t socket<TIP, protocol_t::UDP>::send_to_(
      address_t<TIP> const& address,
      std::byte const* const data,
      std::size_t const data_length
  )
  {
    auto const addr = make_sock_addr(address);
    auto const result = ::sendto(handle_, reinterpret_cast<traits::send_buf_t>(data),
        static_cast<traits::buflen_t>(data_length), 0, reinterpret_cast<sockaddr const*>(&addr),
        static_cast<traits::socklen_t>(sizeof(addr)));
    if (result==-1) {
      throw socket_error{};
    }
    return static_cast<std::size_t>(result);
  }

  template<ip_version_t TIP>
  std::size_t socket<TIP, protocol_t::UDP>::receive_from_(
      address_t<TIP>* const address,
      std::byte* const buffer,
      std::size_t const buffer_length
  )
  {
    ::sockaddr_t<TIP> addr{};
    auto addr_len{static_cast<traits::socklen_t>(sizeof(addr))};
    auto const result = ::recvfrom(
        handle_,
        reinterpret_cast<traits::recv_buf_t>(buffer),
        static_cast<traits::buflen_t>(buffer_length),
        0,
        reinterpret_cast<sockaddr*>(&addr),
        &addr_len);

    if (address!=nullptr) {
      *address = make_address(addr);
    }

    if (result==-1) {
      throw socket_error{};
    }

    return static_cast<std::size_t>(result);
  }

  template
  class socket<ip_version_t::V4, protocol_t::TCP>;

  template
  class socket<ip_version_t::V4, protocol_t::UDP>;

  template
  class socket<ip_version_t::V6, protocol_t::TCP>;

  template
  class socket<ip_version_t::V6, protocol_t::UDP>;
}
