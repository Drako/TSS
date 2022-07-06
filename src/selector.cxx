#include <tss/selector.hxx>
#include <tss/exceptions.hxx>

#if defined(_WIN32)

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <Windows.h>
#include <WinSock2.h>

#else

#include <sys/select.h>

#endif

#include <algorithm>

namespace tss {
  namespace detail {
    struct selector_data final {
      int nfds{};
      fd_set readfds{};
      fd_set writefds{};
      fd_set exceptfds{};
    };
  }

  selector::selector(native::socket_api const&)
      :data_{std::make_unique<detail::selector_data>()}
  {
  }

  selector::~selector() noexcept
  = default;

  void selector::add_read_(gsl::span<traits::socket_t const> const sockets) noexcept
  {
    for (auto const sock: sockets) {
      FD_SET(sock, &data_->readfds);
      data_->nfds = std::max(data_->nfds, static_cast<int>(sock));
    }
  }

  void selector::add_write_(gsl::span<traits::socket_t const> const sockets) noexcept
  {
    for (auto const sock: sockets) {
      FD_SET(sock, &data_->writefds);
      data_->nfds = std::max(data_->nfds, static_cast<int>(sock));
    }
  }

  void selector::add_except_(gsl::span<traits::socket_t const> const sockets) noexcept
  {
    for (auto const sock: sockets) {
      FD_SET(sock, &data_->exceptfds);
      data_->nfds = std::max(data_->nfds, static_cast<int>(sock));
    }
  }

  std::size_t selector::select(std::chrono::microseconds time_out)
  {
    timeval tv{};
    tv.tv_sec = static_cast<decltype(tv.tv_sec)>(time_out.count()/1'000'000);
    tv.tv_usec = static_cast<decltype(tv.tv_usec)>(time_out.count()%1'000'000);

    auto const result = ::select(data_->nfds+1, &data_->readfds, &data_->writefds, &data_->exceptfds, &tv);
    if (result==-1) {
      throw socket_error{};
    }

    return static_cast<std::size_t>(result);
  }

  bool selector::is_read_(traits::socket_t const sock) const noexcept
  {
    return FD_ISSET(sock, &data_->readfds);
  }

  bool selector::is_write_(traits::socket_t const sock) const noexcept
  {
    return FD_ISSET(sock, &data_->writefds);
  }

  bool selector::is_except_(traits::socket_t const sock) const noexcept
  {
    return FD_ISSET(sock, &data_->exceptfds);
  }

  void selector::clear() noexcept
  {
    data_->nfds = 0;
    FD_ZERO(&data_->readfds);
    FD_ZERO(&data_->writefds);
    FD_ZERO(&data_->exceptfds);
  }
}
