#include <gtest/gtest.h>

#include <tss/exceptions.hxx>

TEST(ExceptionsTests, baseExceptionIsCopyable)
{
  tss::exception const ex{"test"};
  tss::exception const ex2{ex};
  EXPECT_STREQ(ex.what(), ex2.what());
}

TEST(ExceptionsTests, socketErrorIsCopyable)
{
  tss::socket_error const ex{};
  tss::socket_error const ex2{ex};
  EXPECT_STREQ(ex.what(), ex2.what());
}

TEST(ExceptionsTests, addressInfoErrorIsCopyable)
{
  tss::address_info_error const ex{0};
  tss::address_info_error const ex2{ex};
  EXPECT_STREQ(ex.what(), ex2.what());
}
