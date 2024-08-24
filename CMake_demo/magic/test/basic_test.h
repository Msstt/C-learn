#include "gtest/gtest.h"
#include "magic.h"

TEST(math, basic) {
    ASSERT_EQ(Magic::fun(1), 2);
}