#pragma once

#include <stdexcept>
#include <string>

namespace tss {
  class exception : public std::runtime_error {
  public:
    using std::runtime_error::runtime_error;
    using std::runtime_error::operator=;
  };

  class socket_error : public exception {
  public:
    socket_error();

    explicit socket_error(int error_code);

    socket_error(socket_error const&) noexcept = default;

    socket_error& operator=(socket_error const&) noexcept = default;

    [[nodiscard]] int error_code() const noexcept;

  private:
    int error_code_;
  };
}
