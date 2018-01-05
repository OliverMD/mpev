//
// Created by odownard on 03/11/17.
//
#include <include/PopulationStates.h>
#include "gtgTest.h"
#include "include/Fitness.h"
#include "include/Population.h"

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

  fitman.changeFitnessFunction(
      [](const IndividualRep *a, const IndividualRep *b) { return (float)88; });

  // Test iterator
  auto seqOne = fitman.readySignal(&popOne);
  auto seqTwo = fitman.readySignal(&popTwo);

  auto iter = fitman.begin(popOne.getId());
  EXPECT_EQ(static_cast<IntIndividualRep *>(*iter)->getValue(), 48);
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

  fitman.changeFitnessFunction(
      [](const IndividualRep *a, const IndividualRep *b) { return (float)88; });

  // Test iterator
  auto seqOne = fitman.readySignal(&popOne);
  auto seqTwo = fitman.readySignal(&popTwo);

  EXPECT_EQ(seqOne, 0);
  EXPECT_EQ(seqTwo, 0);

  EXPECT_EQ(popOne.begin()->fitness, 88);
  EXPECT_EQ(popTwo.begin()->fitness, 88);
}

Individual makeIntIndividual() {
  static int val = 0;
  return {std::make_unique<IntIndividualRep>(++val), 0};
}

TEST(BasicTests, GeneratePopState) {
  Context ctx;
  ctx.popSize = 3;
  ctx.individualMaker = &makeIntIndividual;
  Population newPop(std::make_unique<InitialPopState>(ctx));

  newPop.step();
  newPop.step();

  EXPECT_EQ(newPop.getState()->name(), EvaluateFitnessState::Name);

  int testVal = 0;
  for (const auto &ind : newPop) {
    EXPECT_EQ(ind.representation->name(), "int");
    EXPECT_EQ(
        static_cast<IntIndividualRep *>(ind.representation.get())->getValue(),
        ++testVal);
  }
}