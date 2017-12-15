/*
 * AllTest.cpp
 *
 *  Created on: Jan 20, 2017
 *      Author: builder
 */
#include "gtest/gtest.h"
#include "gmock/gmock.h"

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	::testing::InitGoogleMock(&argc, argv);
	//	Чтобы запустить конкретный класс-тестер
	//::testing::GTEST_FLAG(filter) = "TargetInofCheckerTest*";
	return RUN_ALL_TESTS();
}

