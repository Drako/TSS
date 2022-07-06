cmake_minimum_required(VERSION 3.20)
cmake_policy(VERSION 3.20)

if (NOT DEFINED PROJECT_NAME)
  set(TSS_IS_ROOT ON)
else ()
  set(TSS_IS_ROOT OFF)
endif ()

project(type_safe_sockets VERSION 0.1.0.0)

include(FetchContent)
FetchContent_Declare(
    GoogleTest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG e2239ee6043f73722e7aa812a459f54a28552929
    # release-1.11.0
)
FetchContent_Declare(
    GSL
    GIT_REPOSITORY https://github.com/microsoft/GSL.git
    GIT_TAG a3534567187d2edc428efd3f13466ff75fe5805c
    # v4.0.0
)
FetchContent_MakeAvailable(GSL)

add_library(tss STATIC
    include/tss/address.hxx src/address.cxx
    include/tss/concepts.hxx
    include/tss/enums.hxx
    include/tss/exceptions.hxx src/exceptions.cxx
    include/tss/native.hxx
    include/tss/socket.hxx src/socket.cxx
    include/tss/selector.hxx src/selector.cxx
    include/tss/traits.hxx
    )
target_compile_features(tss PUBLIC cxx_std_20)
target_include_directories(tss PUBLIC "${CMAKE_CURRENT_LIST_DIR}/include")
target_link_libraries(tss PUBLIC Microsoft.GSL::GSL)
if (WIN32)
  target_sources(tss PRIVATE src/socket_api_win32.cxx)
  target_link_libraries(tss PUBLIC ws2_32)
else()
  target_sources(tss PRIVATE src/socket_api_stub.cxx)
endif ()

add_library(tss::tss ALIAS tss)

if (TSS_IS_ROOT)
  enable_testing()
  FetchContent_MakeAvailable(GoogleTest)

  add_executable(tss_tests
      tests/address_tests.cxx
      tests/exceptions_tests.cxx
      tests/socket_tests.cxx)
  target_link_libraries(tss_tests PRIVATE tss gtest gmock gmock_main)
  add_test(NAME tss_tests COMMAND tss_tests)
endif ()
