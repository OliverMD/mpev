//
// Created by odownard on 03/11/17.
//
#include "gtgTest.h"
#include "include/Population.h"

TEST(BasicTests, Sanity) { EXPECT_EQ(0, 0); }

TEST(BasicTests, Population) {
  Population pop;
  EXPECT_EQ("InitialPopState", pop.getState()->name());
  Population newPop(std::make_unique<InitialPopState>());
  EXPECT_EQ("InitialPopState", newPop.getState()->name());
}