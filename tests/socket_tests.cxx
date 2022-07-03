#include <gtest/gtest.h>

#include <tss/selector.hxx>
#include <tss/socket.hxx>

#include <atomic>
#include <future>
#include <thread>
#include <vector>

TEST(SocketTests, canSendAndReceiveOverUdp4)
{
  tss::address_v4_t const address{{127U, 0U, 0U, 1U}, 12345U};
  std::atomic done{false};

  std::future<int> answer{std::async(std::launch::async, [address, &done] {
    tss::udp_socket_4 sock{};
    sock.bind(address);

    int value{};

    tss::selector selector{};
    while (!done) {
      selector.clear();
      selector.add_read(sock);

      selector.select(std::chrono::milliseconds{100U});

      if (selector.is_read(sock)) {
        auto const received = sock.receive_from(nullptr, value);
        EXPECT_EQ(received, sizeof(int));

        done = true;
      }
    }

    return value;
  })};

  std::thread sender{[address, &done] {
    tss::udp_socket_4 sock{};
    int const data = 42;

    while (!done) {
      std::this_thread::sleep_for(std::chrono::milliseconds{250U});
      sock.send_to(address, data);
    }
  }};

  if (auto const result = answer.wait_for(std::chrono::seconds{2}); result==std::future_status::timeout) {
    done = true;
  }
  sender.join();

  EXPECT_EQ(answer.get(), 42);
}

TEST(SocketTests, canSendAndReceiveOverUdp6)
{
  tss::address_v6_t const address{{0U, 0U, 0U, 0U, 0U, 0U, 0U, 1U}, 12345U};
  std::atomic done{false};

  std::future<int> answer{std::async(std::launch::async, [address, &done] {
    tss::udp_socket_6 sock{};
    sock.bind(address);

    int value{};

    tss::selector selector{};
    while (!done) {
      selector.clear();
      selector.add_read(sock);

      selector.select(std::chrono::milliseconds{100U});

      if (selector.is_read(sock)) {
        auto const received = sock.receive_from(nullptr, value);
        EXPECT_EQ(received, sizeof(int));

        done = true;
      }
    }

    return value;
  })};

  std::thread sender{[address, &done] {
    tss::udp_socket_6 sock{};
    int const data = 42;

    while (!done) {
      std::this_thread::sleep_for(std::chrono::milliseconds{250U});
      sock.send_to(address, data);
    }
  }};

  if (auto const result = answer.wait_for(std::chrono::seconds{2}); result==std::future_status::timeout) {
    done = true;
  }
  sender.join();

  EXPECT_EQ(answer.get(), 42);
}

TEST(SocketTests, canSendAndReceiveOverTcp4)
{
  tss::address_v4_t const address{{127U, 0U, 0U, 1U}, 54321U};

  std::thread server([address] {
    tss::tcp_socket_4 sock{};
    sock.set_reuse_addr();
    sock.bind(address);
    sock.listen(5);
    tss::tcp_socket_4 client{sock.accept(nullptr)};
    int value{};
    client.receive(value);
    EXPECT_EQ(value, 23);
    client.send(42);
  });

  std::this_thread::sleep_for(std::chrono::milliseconds{250U});

  std::thread client([address] {
    tss::tcp_socket_4 sock{};
    sock.connect(address);
    sock.send(23);
    int value{};
    sock.receive(value);
    EXPECT_EQ(value, 42);
  });

  server.join();
  client.join();
}

TEST(SocketTests, canSendAndReceiveOverTcp6)
{
  tss::address_v6_t const address{{0U, 0U, 0U, 0U, 0U, 0U, 0U, 1U}, 54321U};

  std::thread server([address] {
    tss::tcp_socket_6 sock{};
    sock.set_reuse_addr();
    sock.bind(address);
    sock.listen(5);
    tss::tcp_socket_6 client{sock.accept(nullptr)};
    int value{};
    client.receive(value);
    EXPECT_EQ(value, 23);
    client.send(42);
  });

  std::this_thread::sleep_for(std::chrono::milliseconds{250U});

  std::thread client([address] {
    tss::tcp_socket_6 sock{};
    sock.connect(address);
    sock.send(23);
    int value{};
    sock.receive(value);
    EXPECT_EQ(value, 42);
  });

  server.join();
  client.join();
}
