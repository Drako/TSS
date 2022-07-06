#include <tss/exceptions.hxx>

#include <utility>

#if defined(_WIN32)

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <WinSock2.h>

#else

#include <cerrno>
#include <cstring>

#endif

namespace {
#if defined(_WIN32)

  int last_error() noexcept
  {
    return WSAGetLastError();
  }

  std::string error_string(int const error_code)
  {
    char* buffer{nullptr};
    if (FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        nullptr,
        static_cast<DWORD>(error_code),
        0U,
        reinterpret_cast<LPSTR>(&buffer),
        256U,
        nullptr
    )==0U) {
      return std::to_string(error_code);
    }
    std::string message{buffer};
    LocalFree(buffer);
    return message;
  }

#else

  int last_error() noexcept
  {
    return errno;
  }

  std::string error_string(int const error_code)
  {
    return std::strerror(error_code);
  }

#endif
}

namespace tss {
  socket_error::socket_error()
      :socket_error{::last_error()}
  {
  }

  socket_error::socket_error(int error_code)
      :exception{::error_string(error_code)}, error_code_{error_code}
  {
  }

  int socket_error::error_code() const noexcept
  {
    return error_code_;
  }
}
