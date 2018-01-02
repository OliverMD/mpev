//
// Created by odownard on 03/11/17.
//
#include "gtgTest.h"
#include "include/Population.h"
#include "include/Fitness.h"

TEST(BasicTests, Sanity) { EXPECT_EQ(0, 0); }

TEST(BasicTests, Population) {
  Context ctx;
  Population newPop(std::make_unique<InitialPopState>(ctx));
  EXPECT_EQ("InitialPopState", newPop.getState()->name());

  std::vector<Individual> test;
  const float fit = 45.45;
  test.emplace_back(std::make_unique<IntIndividualRep>(34), fit);

  newPop.replacePopulation(std::move(test));

  EXPECT_EQ(newPop.begin()->fitness, fit);
}

TEST(BasicTests, FitnessManagerIterator) {
  FitnessManager fitman{3};
  Context ctx;

  Population popOne{std::make_unique<InitialPopState>(ctx)};
  Population popTwo{std::make_unique<InitialPopState>(ctx)};

  std::vector<Individual> test;

  test.emplace_back(std::make_unique<IntIndividualRep>(34), 0);
  popOne.replacePopulation(std::move(test));

  test.clear();

  test.emplace_back(std::make_unique<IntIndividualRep>(48), 0);
  popTwo.replacePopulation(std::move(test));

  fitman.changeFitnessFunction([](const IndividualRep * a,
                                  const IndividualRep * b){
    return (float)88;
  });

  // Test iterator
  auto seqOne = fitman.readySignal(&popOne);
  auto seqTwo = fitman.readySignal(&popTwo);

  auto iter = fitman.begin(popOne.getId());
  EXPECT_EQ(static_cast<IntIndividualRep*>(*iter)->getValue(), 48);
  ++iter;
  EXPECT_EQ(iter == fitman.end(popOne.getId()), true);
}

TEST(BasicTests, FitnessManager) {
  FitnessManager fitman{2};

  Context ctx;

  Population popOne{std::make_unique<InitialPopState>(ctx)};
  Population popTwo{std::make_unique<InitialPopState>(ctx)};

  std::vector<Individual> test;

  test.emplace_back(std::make_unique<IntIndividualRep>(34), 0);
  popOne.replacePopulation(std::move(test));

  test.clear();

  test.emplace_back(std::make_unique<IntIndividualRep>(48), 0);
  popTwo.replacePopulation(std::move(test));

  fitman.changeFitnessFunction([](const IndividualRep * a,
                                  const IndividualRep * b){
    return (float)88;
  });

  // Test iterator
  auto seqOne = fitman.readySignal(&popOne);
  auto seqTwo = fitman.readySignal(&popTwo);

  EXPECT_EQ(seqOne, 0);
  EXPECT_EQ(seqTwo, 1);

  EXPECT_EQ(popOne.begin()->fitness, 88);
  EXPECT_EQ(popTwo.begin()->fitness, 88);
}