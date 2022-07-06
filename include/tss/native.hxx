#pragma once

#include "traits.hxx"

namespace tss::native {
#if defined(_WIN32)
  api_t constexpr api = api_t::WinSock;
#else
  api_t constexpr api = api_t::Berkeley;
#endif

  using socket_traits = ::tss::socket_traits<api>;

  struct socket {
    using traits = native::socket_traits;

    virtual ~socket() noexcept = default;

    [[nodiscard]] virtual traits::socket_t native_handle() const noexcept = 0;
  };

  class socket_api final {
  public:
    socket_api(socket_api const&) = delete;

    socket_api& operator=(socket_api const&) = delete;

    ~socket_api() noexcept;

    static socket_api const& instance();

  private:
    socket_api();
  };
}
