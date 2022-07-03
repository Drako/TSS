#include <tss/exceptions.hxx>

#include <utility>

#if defined(_WIN32)
#error TODO: implement windows version
#else

#include <cerrno>
#include <cstring>

#endif

namespace {
#if defined(_WIN32)
#error TODO: implement windows version
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
