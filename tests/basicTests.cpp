//
// Created by odownard on 03/11/17.
//
#include "gtgTest.h"
#include "include/Fitness.h"
#include "include/Population.h"
#include <include/PopulationStates.h>

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
  auto fit = [](const IndividualRep *a, const IndividualRep *b) {
    return (float)88;
  };
  CoevFitnessManager<DefaultFitnessEv<fit>> fitman{3, 1};
  Context ctx;

  Population popOne{std::make_unique<InitialPopState>(ctx)};
  Population popTwo{std::make_unique<InitialPopState>(ctx)};

  std::vector<Individual> test;

  test.emplace_back(std::make_unique<IntIndividualRep>(34), 0);
  popOne.replacePopulation(std::move(test));

  test.clear();

  test.emplace_back(std::make_unique<IntIndividualRep>(48), 0);
  popTwo.replacePopulation(std::move(test));

  // Test iterator
  auto seqOne = fitman.readySignal(&popOne);
  auto seqTwo = fitman.readySignal(&popTwo);

  auto iter = fitman.begin(popOne.getId());
  EXPECT_EQ(static_cast<IntIndividualRep *>(*iter)->getValue(), 48);
  ++iter;
  EXPECT_EQ(iter == fitman.end(popOne.getId()), true);
}

TEST(BasicTests, FitnessManager) {
  auto fit = [](const IndividualRep *a, const IndividualRep *b) {
    return (float)88;
  };
  CoevFitnessManager<DefaultFitnessEv<fit>> fitman{2, 1};

  Context ctx;

  Population popOne{std::make_unique<InitialPopState>(ctx)};
  Population popTwo{std::make_unique<InitialPopState>(ctx)};

  std::vector<Individual> test;

  test.emplace_back(std::make_unique<IntIndividualRep>(34), 0);
  popOne.replacePopulation(std::move(test));

  test.clear();

  test.emplace_back(std::make_unique<IntIndividualRep>(48), 0);
  popTwo.replacePopulation(std::move(test));

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

TEST(BasicTests, EvalFitnessPopState_OnePop) {
  Context ctx;
  auto fit = [](const IndividualRep *, const IndividualRep *) -> float {
    return 50;
  };
  ctx.popSize = 3;
  ctx.individualMaker = &makeIntIndividual;
  ctx.fitnessManager =
      std::make_unique<CoevFitnessManager<DefaultFitnessEv<fit>>>(1, 1);
  Population newPop(std::make_unique<InitialPopState>(ctx));

  newPop.step();
  newPop.step();

  EXPECT_EQ(newPop.getState()->name(), EvaluateFitnessState::Name);

  newPop.step();

  EXPECT_EQ(newPop.getState()->name(), VariationState::Name);

  int testVal = 0;
  for (const auto &ind : newPop) {
    EXPECT_EQ(ind.representation->name(), "int");
    EXPECT_EQ(
        static_cast<IntIndividualRep *>(ind.representation.get())->getValue(),
        ++testVal);
    EXPECT_EQ(ind.fitness, 50);
  }
}

TEST(BasicTests, VariationPopState_OnePop) {
  Context ctx;
  auto fit = [](const IndividualRep *, const IndividualRep *) -> float {
    return 50;
  };
  ctx.popSize = 3;
  ctx.individualMaker = &makeIntIndividual;
  ctx.fitnessManager =
      std::make_unique<CoevFitnessManager<DefaultFitnessEv<fit>>>(1, 1);
  Population newPop(std::make_unique<InitialPopState>(ctx));

  newPop.step();
  newPop.step();

  EXPECT_EQ(newPop.getState()->name(), EvaluateFitnessState::Name);

  newPop.step();

  EXPECT_EQ(newPop.getState()->name(), VariationState::Name);

  int testVal = 0;
  for (const auto &ind : newPop) {
    EXPECT_EQ(ind.representation->name(), "int");
    EXPECT_EQ(
        static_cast<IntIndividualRep *>(ind.representation.get())->getValue(),
        ++testVal);
    EXPECT_EQ(ind.fitness, 50);
  }

  newPop.step();

  EXPECT_EQ(newPop.size(), newPop.newInds.size());
}

TEST(BasicTests, VariationPopState_TwoPop) {
  const float expFitness = 50;
  auto fit = [](const IndividualRep *, const IndividualRep *) -> float {
    return 50;
  };
  Context ctx;
  ctx.popSize = 3;
  ctx.individualMaker = &makeIntIndividual;
  ctx.fitnessManager =
      std::make_unique<CoevFitnessManager<DefaultFitnessEv<fit>>>(2, 1);

  Population newPop(std::make_unique<InitialPopState>(ctx));
  Population popTwo(std::make_unique<InitialPopState>(ctx));

  newPop.step();
  popTwo.step();
  newPop.step();
  popTwo.step();

  EXPECT_EQ(newPop.getState()->name(), EvaluateFitnessState::Name);
  EXPECT_EQ(popTwo.getState()->name(), EvaluateFitnessState::Name);

  newPop.step();
  popTwo.step();

  EXPECT_EQ(popTwo.getState()->name(), VariationState::Name);
  newPop.step(); // Need to step to pickup change
  EXPECT_EQ(newPop.getState()->name(), VariationState::Name);

  int testVal = 0;
  for (const auto &ind : newPop) {
    EXPECT_EQ(ind.representation->name(), "int");
    EXPECT_EQ(
        static_cast<IntIndividualRep *>(ind.representation.get())->getValue(),
        ++testVal);
    EXPECT_EQ(ind.fitness, 50);
  }

  testVal = 0;
  for (const auto &ind : popTwo) {
    EXPECT_EQ(ind.representation->name(), "int");
    EXPECT_EQ(
        static_cast<IntIndividualRep *>(ind.representation.get())->getValue(),
        ++testVal);
    EXPECT_EQ(ind.fitness, 50);
  }

  newPop.step();
  popTwo.step();

  EXPECT_EQ(newPop.size(), newPop.newInds.size());
  EXPECT_EQ(popTwo.size(), popTwo.newInds.size());
}