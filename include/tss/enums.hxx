#pragma once

#include <cstdint>

namespace tss {
  enum class ip_version_t : std::uint8_t {
    V4 = 4U,
    V6 = 6U,
  };

  enum class protocol_t : std::uint8_t {
    TCP,
    UDP,
  };

  enum class api_t : std::uint8_t {
    WinSock,
    Berkeley,
  };

  enum class shutdown_t : std::uint8_t {
    Read = 1U,
    Write = 2U,
    ReadWrite = 3U,
  };
}
