#include <tss/native.hxx>
#include <tss/exceptions.hxx>

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <WinSock2.h>

namespace tss::native {
  socket_api::socket_api()
  {
    WSADATA data{};
    auto const result = WSAStartup(MAKEWORD(2, 1), &data);
    if (result!=0) {
      throw socket_error{result};
    }
  }

  socket_api::~socket_api() noexcept
  {
    WSACleanup();
  }

  socket_api const& socket_api::instance()
  {
    static socket_api api{};
    return api;
  }
}
