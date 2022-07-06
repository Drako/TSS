#pragma once

#include "address.hxx"
#include "concepts.hxx"
#include "enums.hxx"
#include "native.hxx"

#include <array>
#include <utility>

namespace tss {
  namespace detail {
    template<ip_version_t TIP, protocol_t TProto>
    class socket_base : public native::socket {
    public:
      /**
       * Default constructs a socket with the given IP address version and protocol.
       * @throws socket_error If the native socket call fails.
       */
      explicit socket_base(native::socket_api const& = native::socket_api::instance());

      /**
       * Move constructs a socket making the original socket invalid.
       * @param src The original socket.
       */
      socket_base(socket_base&& src) noexcept;

      /**
       * Copy construction is disabled.
       */
      socket_base(socket_base const&) = delete;

      /**
       * Reassignment is disabled.
       */
      socket_base& operator=(socket_base&&) = delete;

      /**
       * Reassignment is disabled.
       */
      socket_base& operator=(socket_base const&) = delete;

      /**
       * The destructor closes the socket.
       */
      ~socket_base() noexcept override;

      /**
       * Access the native socket handle.
       * @return The native socket handle.
       */
      [[nodiscard]] traits::socket_t native_handle() const noexcept override;

      /**
       * Check whether the socket can be used.
       * @return true, if the socket is valid and can be used, false otherwise.
       */
      [[nodiscard]] bool is_valid() const noexcept;

      /**
       * Close the native socket and invalidate it.
       * @throws socket_error If the socket was valid, but could not be closed.
       */
      void close();

      /**
       * Bind the socket to the given IP address and port.
       * @param address The IP address and port to bind to.
       * @throws socket_error If the native bind call fails.
       */
      void bind(address_t<TIP> const& address);

      /**
       * Allow binding to already used address.
       * @param reuse Whether reusing the same address should be allowed.
       * @throws socket_error If the native setsockopt call fails.
       */
      void set_reuse_addr(bool reuse = true);

      /**
       * Check whether reusing the same address is currently allowed.
       * @return true, if reusing address is currently allowed, false otherwise.
       * @throws socket_error If the native getsockopt call fails.
       */
      [[nodiscard]] bool get_reuse_addr() const;

    protected:
      using traits = native::socket_traits;
      traits::socket_t handle_;

      explicit socket_base(traits::socket_t handle) noexcept;
    };

    extern template
    class socket_base<ip_version_t::V4, protocol_t::TCP>;

    extern template
    class socket_base<ip_version_t::V4, protocol_t::UDP>;

    extern template
    class socket_base<ip_version_t::V6, protocol_t::TCP>;

    extern template
    class socket_base<ip_version_t::V6, protocol_t::UDP>;
  }

  template<ip_version_t TIP, protocol_t TProto>
  class socket;

  template<ip_version_t TIP>
  class socket<TIP, protocol_t::TCP> final : public detail::socket_base<TIP, protocol_t::TCP> {
    using base_t = detail::socket_base<TIP, protocol_t::TCP>;
    using traits = native::socket_traits;
    using base_t::handle_;

  public:
    using base_t::base_t;
    using base_t::close;
    using base_t::bind;
    using base_t::is_valid;

    /**
     * Listen for connections.
     * @param backlog Maximum number of queued connections.
     * @throws socket_error If the native listen call fails.
     */
    void listen(std::uint32_t backlog);

    /**
     * Establish a connection to a server.
     * @param address The address of the server consisting of IP address and port number.
     * @throws socket_error If the native connect call fails.
     */
    void connect(tss::address_t<TIP> const& address);

    /**
     * Accept a connection.
     * @param address The address of the connecting client. Can be nullptr if irrelevant.
     * @return The socket for the new connection.
     * @throws socket_error If the native accept call fails.
     */
    socket accept(address_t<TIP>* address);

    /**
     * Shuts down all or part of a full-duplex connection.
     * @param how Which parts to shut down.
     * @throws socket_error If the native shutdown call fails.
     */
    void shutdown(shutdown_t how);

    /**
     * Send data to the connected peer.
     * @tparam TData The type of data to send.
     * @param data The data to send.
     * @return The number of bytes actually transmitted.
     * @throws socket_error If the native send call fails.
     */
    template<concepts::Data TData>
    std::size_t send(TData const& data)
    {
      return send_(reinterpret_cast<std::byte const*>(std::addressof(data)), sizeof(TData));
    }

    /**
     * Receive data from the connected peer.
     * @tparam TData The type of data to receive.
     * @param buffer The buffer receiving the incoming data.
     * @return The number of bytes actually received.
     * @throws socket_error If the native recv call fails.
     */
    template<concepts::Data TData>
    std::size_t receive(TData& buffer)
    {
      return receive_(reinterpret_cast<std::byte*>(std::addressof(buffer)), sizeof(TData));
    }

  private:
    std::size_t send_(std::byte const* data, std::size_t data_length);

    std::size_t receive_(std::byte* buffer, std::size_t buffer_length);
  };

  template<ip_version_t TIP>
  class socket<TIP, protocol_t::UDP> final : public detail::socket_base<TIP, protocol_t::UDP> {
    using base_t = detail::socket_base<TIP, protocol_t::UDP>;
    using traits = native::socket_traits;
    using base_t::handle_;

  public:
    using base_t::base_t;
    using base_t::close;
    using base_t::bind;
    using base_t::is_valid;

    /**
     * Send data to the given address.
     * @tparam TData The type of data to send.
     * @param address The target address consisting of an IP address and a port number.
     * @param data The data to send.
     * @return The number of bytes actually transmitted.
     * @throws socket_error If the native sendto call fails.
     */
    template<concepts::Data TData>
    std::size_t send_to(address_t<TIP> const& address, TData const& data)
    {
      return send_to_(address, reinterpret_cast<std::byte const*>(std::addressof(data)), sizeof(TData));
    }

    /**
     * Receive data from somewhere.
     * @tparam TData The type of data to receive.
     * @param address The sender address. Can be nullptr if irrelevant.
     * @param buffer The buffer receiving the incoming data.
     * @return The number of bytes actually received.
     * @throws socket_error If the native recvfrom call fails.
     */
    template<concepts::Data TData>
    std::size_t receive_from(address_t<TIP>* address, TData& buffer)
    {
      return receive_from_(address, reinterpret_cast<std::byte*>(std::addressof(buffer)), sizeof(TData));
    }

  private:
    std::size_t send_to_(address_t<TIP> const& address, std::byte const* data, std::size_t data_length);

    std::size_t receive_from_(address_t<TIP>* address, std::byte* buffer, std::size_t buffer_length);
  };

  extern template
  class socket<ip_version_t::V4, protocol_t::TCP>;

  extern template
  class socket<ip_version_t::V4, protocol_t::UDP>;

  extern template
  class socket<ip_version_t::V6, protocol_t::TCP>;

  extern template
  class socket<ip_version_t::V6, protocol_t::UDP>;

  using tcp_socket_4 = socket<ip_version_t::V4, protocol_t::TCP>;
  using tcp_socket_6 = socket<ip_version_t::V6, protocol_t::TCP>;
  using udp_socket_4 = socket<ip_version_t::V4, protocol_t::UDP>;
  using udp_socket_6 = socket<ip_version_t::V6, protocol_t::UDP>;
}
