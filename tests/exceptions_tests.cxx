#include <gtest/gtest.h>

#include <tss/exceptions.hxx>

TEST(ExceptionsTests, baseExceptionIsCopyable)
{
  tss::exception const ex{"test"};
  tss::exception const ex2{ex};
  EXPECT_EQ(ex.what(), ex2.what());
}

TEST(ExceptionsTests, socketErrorIsCopyable)
{
  tss::socket_error const ex{};
  tss::socket_error const ex2{ex};
  EXPECT_EQ(ex.what(), ex2.what());
}
