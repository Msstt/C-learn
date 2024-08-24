#include "gtest/gtest.h"
#include "mmath.h"

TEST(math, basic) {
    ASSERT_EQ(Math::add(1, 2), 3);
}