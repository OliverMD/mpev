//
// Created by odownard on 03/11/17.
//
#include "gtgTest.h"
#include "include/Fitness.h"
#include <include/PopulationStates.h>

using FEV = DefaultFitnessEv<identityFitnessFunc>;

TEST(BasicTests, Sanity) { EXPECT_EQ(0, 0); }

TEST(BasicTests, Population) {
  Context<DefaultFitnessEv<identityFitnessFunc>> ctx;
  Population<FEV> newPop(std::make_unique<InitialPopState<FEV>>(ctx));
  EXPECT_EQ("InitialPopState", newPop.getState()->name());

  std::vector<Individual> test;
  const float fit = 45.45;
  test.emplace_back(std::make_unique<IntIndividualRep>(34), fit);

  newPop.replacePopulation(std::move(test));

  EXPECT_EQ(newPop.begin()->fitness, fit);
}

TEST(BasicTests, FitnessManagerIterator) {
  FitnessManager<FEV> fitman{3, {}, 1};
  Context<FEV> ctx;

  Population<FEV> popOne{std::make_unique<InitialPopState<FEV>>(ctx)};
  Population<FEV> popTwo{std::make_unique<InitialPopState<FEV>>(ctx)};

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
  auto fitfunc = [](const IndividualRep *a, const IndividualRep *b) {
    return (float)88;
  };
  using FE = DefaultFitnessEv<fitfunc>;
  FitnessManager<FE> fitman{2, {}, 1};

  Context<FE> ctx;

  Population<FE> popOne{std::make_unique<InitialPopState<FE>>(ctx)};
  Population<FE> popTwo{std::make_unique<InitialPopState<FE>>(ctx)};

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
  Context<FEV> ctx;
  ctx.popSize = 3;
  ctx.individualMaker = &makeIntIndividual;
  Population<FEV> newPop(std::make_unique<InitialPopState<FEV>>(ctx));

  newPop.step();
  newPop.step();

  EXPECT_EQ(newPop.getState()->name(), EvaluateFitnessState<FEV>::Name);

  int testVal = 0;
  for (const auto &ind : newPop) {
    EXPECT_EQ(ind.representation->name(), "int");
    EXPECT_EQ(
        static_cast<IntIndividualRep *>(ind.representation.get())->getValue(),
        ++testVal);
  }
}

TEST(BasicTests, EvalFitnessPopState_OnePop) {
  auto fitfunc = [](const IndividualRep *, const IndividualRep *) -> float {
    return 50;
  };

  using FE = DefaultFitnessEv<fitfunc>;
  Context<FE> ctx;
  ctx.popSize = 3;
  ctx.individualMaker = &makeIntIndividual;

  ctx.fitnessManager =
      std::make_unique<FitnessManager<FE>>(1, DefaultFitnessEv<fitfunc>{}, 1);

  Population<FE> newPop(std::make_unique<InitialPopState<FE>>(ctx));

  newPop.step();
  newPop.step();

  EXPECT_EQ(newPop.getState()->name(), EvaluateFitnessState<FE>::Name);

  newPop.step();

  EXPECT_EQ(newPop.getState()->name(), VariationState<FE>::Name);

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
  auto fitfunc = [](const IndividualRep *, const IndividualRep *) -> float {
    return 50;
  };

  using FE = DefaultFitnessEv<fitfunc>;
  Context<FE> ctx;
  ctx.popSize = 3;
  ctx.individualMaker = &makeIntIndividual;

  ctx.fitnessManager =
      std::make_unique<FitnessManager<FE>>(1, DefaultFitnessEv<fitfunc>{}, 1);

  Population<FE> newPop(std::make_unique<InitialPopState<FE>>(ctx));

  newPop.step();
  newPop.step();

  EXPECT_EQ(newPop.getState()->name(), EvaluateFitnessState<FE>::Name);

  newPop.step();

  EXPECT_EQ(newPop.getState()->name(), VariationState<FE>::Name);

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
  auto fitfunc = [](const IndividualRep *, const IndividualRep *) -> float {
    return 50;
  };

  using FE = DefaultFitnessEv<fitfunc>;

  Context<FE> ctx;
  ctx.popSize = 3;
  ctx.individualMaker = &makeIntIndividual;
  ctx.fitnessManager = std::make_unique<FitnessManager<FE>>(2, FE{}, 1);

  Population<FE> newPop(std::make_unique<InitialPopState<FE>>(ctx));
  Population<FE> popTwo(std::make_unique<InitialPopState<FE>>(ctx));

  newPop.step();
  popTwo.step();
  newPop.step();
  popTwo.step();

  EXPECT_EQ(newPop.getState()->name(), EvaluateFitnessState<FE>::Name);
  EXPECT_EQ(popTwo.getState()->name(), EvaluateFitnessState<FE>::Name);

  newPop.step();
  popTwo.step();

  EXPECT_EQ(popTwo.getState()->name(), VariationState<FE>::Name);
  newPop.step(); // Need to step to pickup change
  EXPECT_EQ(newPop.getState()->name(), VariationState<FE>::Name);

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