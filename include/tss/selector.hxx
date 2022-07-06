#pragma once

#include "socket.hxx"
#include "concepts.hxx"

#include <array>
#include <chrono>
#include <memory>

#include <gsl/span>

namespace tss {
  namespace detail {
    struct selector_data;
  }

  class selector final {
  public:
    selector();

    ~selector() noexcept;

    template<concepts::Socket... TSocket>
    void add_read(TSocket const& ... socket) noexcept
    {
      std::array<traits::socket_t, sizeof...(TSocket)> sockets{socket.native_handle()...};
      add_read_(sockets);
    }

    template<concepts::Socket... TSocket>
    void add_write(TSocket const& ... socket) noexcept
    {
      std::array<traits::socket_t, sizeof...(TSocket)> sockets{socket.native_handle()...};
      add_write_(sockets);
    }

    template<concepts::Socket... TSocket>
    void add_except(TSocket const& ... socket) noexcept
    {
      std::array<traits::socket_t, sizeof...(TSocket)> sockets{socket.native_handle()...};
      add_except_(sockets);
    }

    std::size_t select(std::chrono::microseconds time_out = std::chrono::microseconds{0});

    void clear() noexcept;

    template<concepts::Socket TSocket>
    [[nodiscard]] bool is_read(TSocket const& sock) const noexcept
    {
      return is_read_(sock.native_handle());
    }

    template<concepts::Socket TSocket>
    [[nodiscard]] bool is_write(TSocket const& sock) const noexcept
    {
      return is_write_(sock.native_handle());
    }

    template<concepts::Socket TSocket>
    [[nodiscard]] bool is_except(TSocket const& sock) const noexcept
    {
      return is_except_(sock.native_handle());
    }

  private:
    using traits = native::socket_traits;

    void add_read_(gsl::span<traits::socket_t const> sockets) noexcept;

    void add_write_(gsl::span<traits::socket_t const> sockets) noexcept;

    void add_except_(gsl::span<traits::socket_t const> sockets) noexcept;

    [[nodiscard]] bool is_read_(traits::socket_t sock) const noexcept;

    [[nodiscard]] bool is_write_(traits::socket_t sock) const noexcept;

    [[nodiscard]] bool is_except_(traits::socket_t sock) const noexcept;

    std::unique_ptr<detail::selector_data> data_;
  };
}
