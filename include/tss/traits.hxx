#pragma once

#include "enums.hxx"

#include <cstddef>
#include <cstdint>
#include <limits>

namespace tss {
  template<api_t TAPI>
  struct socket_traits;

  template<>
  struct socket_traits<api_t::WinSock> final {
    using socket_t = std::uint64_t;
    using socklen_t = int;
    using send_buf_t = char const*;
    using recv_buf_t = char*;
    using buflen_t = int;
    static socket_t inline invalid_value = std::numeric_limits<socket_t>::max();
    static std::size_t inline fd_set_size = 64U;
  };

  template<>
  struct socket_traits<api_t::Berkeley> final {
    using socket_t = int;
    using socklen_t = std::uint32_t;
    using send_buf_t = void const*;
    using recv_buf_t = void*;
    using buflen_t = std::size_t;
    static socket_t inline invalid_value = -1;
    static std::size_t inline fd_set_size = 1024U;
  };
}
