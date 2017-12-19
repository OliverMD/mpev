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

  std::vector<Individual> test;
  const float fit = 45.45;
  test.emplace_back(std::make_unique<IntIndividualRep>(34), fit);

  newPop.replacePopulation(std::move(test));

  EXPECT_EQ(newPop.getReadOnlyPopulation()[0].fitness, fit);
}