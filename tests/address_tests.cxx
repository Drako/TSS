#include <gtest/gtest.h>

#include <tss/address.hxx>

TEST(AddressTest, canResolveLocalhost)
{
  auto const addresses = tss::resolve_ip_addresses("localhost");
  EXPECT_EQ(addresses.size(), 2U); // ::1 and 127.0.0.1
}
