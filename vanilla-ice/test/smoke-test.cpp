#include "gmock/gmock.h"

namespace vanilla_ice {

TEST(SmokeTest, BasicAssertions) {
  EXPECT_STRNE("foo", "bar");
  EXPECT_STREQ("Hello", "Hello");

  EXPECT_EQ(7 * 3 * 2, 42);
  EXPECT_EQ(sizeof(char), sizeof(uint8_t));
}

}  // namespace vanilla_ice
