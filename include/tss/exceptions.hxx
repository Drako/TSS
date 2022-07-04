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

  class address_info_error : public exception {
  public:
    explicit address_info_error(int error_code);

    address_info_error(address_info_error const&) noexcept = default;

    address_info_error& operator=(address_info_error const&) noexcept = default;

    [[nodiscard]] int error_code() const noexcept;

  private:
    int error_code_;
  };
}
