#include <iostream>
#include "gtest/gtest.h"
#include "magic.h"

class MyTest : public testing::Test {
  protected:
    static void SetUpTestSuite() {
		std::cout << "Test init" << std::endl;
	}
    static void TearDownTestSuite() {
		std::cout << "Test destroy" << std::endl;
	}
	void SetUp() override {
		std::cout << "Test begin" << std::endl;
	}
	void TearDown() override {
		std::cout << "Test end" << std::endl;
	}
};

TEST_F(MyTest, basic) {
	std::cout << "Testing 1" << std::endl;
	ASSERT_EQ(Magic::fun(1), 2);
}

TEST_F(MyTest, super) {
	std::cout << "Testing 2" << std::endl;
	ASSERT_EQ(Magic::fun(2), 4);
}

void deathFun() {
	exit(-1);
}

TEST(death, basic) {
	ASSERT_DEATH(deathFun(), "");
}

int main(int argc, char** argv) {
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
