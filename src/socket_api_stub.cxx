#include <tss/native.hxx>

namespace tss::native {
  socket_api::socket_api()
  = default;

  socket_api::~socket_api() noexcept
  = default;

  socket_api const& socket_api::instance()
  {
    static socket_api api{};
    return api;
  }
}
