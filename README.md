# Type Safe Sockets

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

This project provides a low level socket wrapper library in modern C++20.
It hides the operating system specific details of different socket APIs behind
a new object-oriented API that should feel quite familiar.

Another goal was to minimize the potential mistakes a developer could make
while using this library.
Therefore, IPv4 sockets only accept IPv4 addresses as parameters and IPv6 sockets only accept IPv6 addresses.
Additionally, certain member functions are only available for UDP sockets, while others are only available for TCP
sockets.

## Usage

### Including into a CMake based project

As TSS is using CMake itself, this is the easiest way to use the library.

```cmake
include(FetchContent)
FetchContent_Declare(
    TSS
    GIT_REPOSITORY https://github.com/Drako/TSS.git
    GIT_TAG <current-version>
)
FetchContent_MakeAvailable(TSS)

add_executable(foo ...)
target_link_libraries(foo PRIVATE tss::tss)
```

### Example TCP Client

```cpp
#include <tss/socket.hxx>
#include <tss/exceptions.hxx>

#include <cstdlib>
#include <iostream>

int main() try {
  tss::tcp_socket_4 sock{};
  sock.connect({{127U, 0U, 0U, 1U}, 1337U});
  sock.send(42);
  
  int response{};
  sock.receive(response);
  
  sock.shutdown(tss::shutdown_t::ReadWrite);
  
  return EXIT_SUCCESS;
}
catch (tss::socket_error const& ex) {
  std::cerr << "A socket operation failed with error code: " << ex.error_code()
    << "\nDescription: " << ex.what() << std::endl;
  return EXIT_FAILURE;
}
```
