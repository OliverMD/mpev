//
// Created by odownard on 03/11/17.
//
#include "gtgTest.h"
#include "include/Fitness.h"
#include <include/PopulationStates.h>
#include "include/Selectors.h"

TEST(BasicTests, Sanity) { EXPECT_EQ(0, 0); }

TEST(BasicTests, Population) {
  Context ctx = makeDefaultContext();
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
  Context ctx = makeDefaultContext();

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

  auto iter = fitman.begin({popOne.getId()});
  EXPECT_EQ(static_cast<IntIndividualRep *>(*iter)->getValue(), 48);
  ++iter;
  EXPECT_EQ(iter == fitman.end({popOne.getId()}), true);
}

TEST(BasicTests, FitnessManager) {
  auto fit = [](const IndividualRep *a, const IndividualRep *b) {
    return (float)88;
  };
  CoevFitnessManager<DefaultFitnessEv<fit>> fitman{2, 1};

  Context ctx = makeDefaultContext();

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
  return {std::make_unique<IntIndividualRep>(5), 0};
}

TEST(BasicTests, GeneratePopState) {
  Context ctx = makeDefaultContext();
  ctx.popSize = 3;
  ctx.individualMaker = &makeIntIndividual;
  Population newPop(std::make_unique<InitialPopState>(ctx));

  newPop.step();
  newPop.step();

  EXPECT_EQ(newPop.getState()->name(), EvaluateFitnessState::Name);

  for (const auto &ind : newPop) {
    EXPECT_EQ(ind.representation->name(), "int");
    EXPECT_EQ(
        static_cast<IntIndividualRep *>(ind.representation.get())->getValue(),
        5);
  }
}

TEST(BasicTests, EvalFitnessPopState_OnePop) {
  Context ctx = makeDefaultContext();
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

  EXPECT_EQ(newPop.getState()->name(), ReporterState::Name);

  for (const auto &ind : newPop) {
    EXPECT_EQ(ind.representation->name(), "int");
    EXPECT_EQ(
        static_cast<IntIndividualRep *>(ind.representation.get())->getValue(),
        5);
    EXPECT_EQ(ind.fitness, 50);
  }
}

TEST(BasicTests, VariationPopState_OnePop) {
  Context ctx = makeDefaultContext();
  auto fit = [](const IndividualRep *, const IndividualRep *) -> float {
    return 50;
  };
  ctx.popSize = 3;
  ctx.individualMaker = &makeIntIndividual;
  ctx.fitnessManager =
      std::make_unique<CoevFitnessManager<DefaultFitnessEv<fit>>>(1, 1);
  ctx.varySelectorCreator = createTournSelect;
  ctx.survivalSelectorCreator = createTournSelect;
  Population newPop(std::make_unique<InitialPopState>(ctx));

  newPop.step();
  newPop.step();

  EXPECT_EQ(newPop.getState()->name(), EvaluateFitnessState::Name);

  newPop.step();

  EXPECT_EQ(newPop.getState()->name(), ReporterState::Name);
  newPop.step();
  EXPECT_EQ(newPop.getState()->name(), VariationState::Name);

  for (const auto &ind : newPop) {
    EXPECT_EQ(ind.representation->name(), "int");
    EXPECT_EQ(
        static_cast<IntIndividualRep *>(ind.representation.get())->getValue(),
        5);
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
  Context ctx = makeDefaultContext();
  ctx.popSize = 3;
  ctx.individualMaker = &makeIntIndividual;
  ctx.fitnessManager =
      std::make_unique<CoevFitnessManager<DefaultFitnessEv<fit>>>(2, 1);
  ctx.crossoverFunc = [](Context &c, Individual &a, Individual &b) {
    std::vector<Individual> ret{};
    ret.emplace_back(std::make_unique<IntIndividualRep>(99), 0);
    return ret;
  };
  ctx.varySelectorCreator = createTournSelect;
  ctx.survivalSelectorCreator = createTournSelect;

  ctx.mutationFunc = [](Individual &a) -> Individual {
    return {std::make_unique<IntIndividualRep>(
                1 + dynamic_cast<IntIndividualRep *>(a.representation.get())
                        ->getValue()),
            0};
  };

  Population newPop(std::make_unique<InitialPopState>(ctx));
  Population popTwo(std::make_unique<InitialPopState>(ctx));

  newPop.step();
  popTwo.step();
  newPop.step();
  popTwo.step();

  EXPECT_EQ(newPop.getState()->name(), EvaluateFitnessState::Name);
  EXPECT_EQ(popTwo.getState()->name(), EvaluateFitnessState::Name);

  EXPECT_GT(newPop.size(), 0);
  EXPECT_GT(popTwo.size(), 0);

  newPop.step();
  popTwo.step();

  EXPECT_EQ(popTwo.getState()->name(), ReporterState::Name);
  newPop.step(); // Need to step to pickup change
  EXPECT_EQ(newPop.getState()->name(), ReporterState::Name);

  EXPECT_GT(newPop.size(), 0);
  EXPECT_GT(popTwo.size(), 0);

  for (const auto &ind : newPop) {
    EXPECT_EQ(ind.representation->name(), "int");
    EXPECT_EQ(
        static_cast<IntIndividualRep *>(ind.representation.get())->getValue(),
        5);
    EXPECT_EQ(ind.fitness, 50);
  }

  for (const auto &ind : popTwo) {
    EXPECT_EQ(ind.representation->name(), "int");
    EXPECT_EQ(
        static_cast<IntIndividualRep *>(ind.representation.get())->getValue(),
        5);
    EXPECT_EQ(ind.fitness, 50);
  }

  newPop.step();
  popTwo.step();

  EXPECT_EQ(popTwo.getState()->name(), VariationState::Name);
  EXPECT_EQ(newPop.getState()->name(), VariationState::Name);

  newPop.step();
  popTwo.step();

  EXPECT_EQ(newPop.getState()->name(), SurvivalState::Name);
  EXPECT_EQ(popTwo.getState()->name(), SurvivalState::Name);

  EXPECT_GT(newPop.size(), 0);
  EXPECT_GT(popTwo.size(), 0);
  for (auto &ind : newPop.newInds) {
    EXPECT_EQ(ind.representation->name(), "int");
    EXPECT_EQ(
        static_cast<IntIndividualRep *>(ind.representation.get())->getValue(),
        100);
    EXPECT_EQ(ind.fitness, 0);
  }

  for (const auto &ind : popTwo.newInds) {
    EXPECT_EQ(ind.representation->name(), "int");
    EXPECT_EQ(
        static_cast<IntIndividualRep *>(ind.representation.get())->getValue(),
        100);
    EXPECT_EQ(ind.fitness, 0);
  }

  EXPECT_EQ(newPop.size(), newPop.newInds.size());
  EXPECT_EQ(popTwo.size(), popTwo.newInds.size());
}

TEST(BasicTests, SurvivalPopState_TwoPop) {
  constexpr size_t popCount = 2;
  const float expFitness = 50;
  auto fit = [](const IndividualRep *, const IndividualRep *) -> float {
    return 50;
  };
  Context ctx = makeDefaultContext();
  ctx.popSize = 3;
  ctx.populationCount = popCount;
  ctx.individualMaker = &makeIntIndividual;
  ctx.fitnessManager =
      std::make_unique<CoevFitnessManager<DefaultFitnessEv<fit>>>(popCount, 1);
  ctx.crossoverFunc = [](Context &c, Individual &a, Individual &b) {
    std::vector<Individual> ret{};
    ret.emplace_back(std::make_unique<IntIndividualRep>(99), 0);
    return ret;
  };
  ctx.varySelectorCreator = createTournSelect;
  ctx.survivalSelectorCreator = createTournSelect;

  ctx.mutationFunc = [](Individual &a) -> Individual {
    return {std::make_unique<IntIndividualRep>(
        1 + dynamic_cast<IntIndividualRep *>(a.representation.get())
            ->getValue()),
            0};
  };

  Population newPop(std::make_unique<InitialPopState>(ctx));
  Population popTwo(std::make_unique<InitialPopState>(ctx));

  newPop.step();
  popTwo.step();
  newPop.step();
  popTwo.step();

  EXPECT_EQ(newPop.getState()->name(), EvaluateFitnessState::Name);
  EXPECT_EQ(popTwo.getState()->name(), EvaluateFitnessState::Name);

  EXPECT_GT(newPop.size(), 0);
  EXPECT_GT(popTwo.size(), 0);

  newPop.step();
  popTwo.step();

  EXPECT_EQ(popTwo.getState()->name(), ReporterState::Name);
  newPop.step(); // Need to step to pickup change
  EXPECT_EQ(newPop.getState()->name(), ReporterState::Name);

  EXPECT_GT(newPop.size(), 0);
  EXPECT_GT(popTwo.size(), 0);

  for (const auto &ind : newPop) {
    EXPECT_EQ(ind.representation->name(), "int");
    EXPECT_EQ(
        static_cast<IntIndividualRep *>(ind.representation.get())->getValue(),
        5);
    EXPECT_EQ(ind.fitness, 50);
  }

  for (const auto &ind : popTwo) {
    EXPECT_EQ(ind.representation->name(), "int");
    EXPECT_EQ(
        static_cast<IntIndividualRep *>(ind.representation.get())->getValue(),
        5);
    EXPECT_EQ(ind.fitness, 50);
  }

  newPop.step();
  popTwo.step();

  EXPECT_EQ(popTwo.getState()->name(), VariationState::Name);
  EXPECT_EQ(newPop.getState()->name(), VariationState::Name);

  newPop.step();
  popTwo.step();

  EXPECT_EQ(newPop.getState()->name(), SurvivalState::Name);
  EXPECT_EQ(popTwo.getState()->name(), SurvivalState::Name);

  EXPECT_GT(newPop.size(), 0);
  EXPECT_GT(popTwo.size(), 0);
  for (auto &ind : newPop.newInds) {
    EXPECT_EQ(ind.representation->name(), "int");
    EXPECT_EQ(
        static_cast<IntIndividualRep *>(ind.representation.get())->getValue(),
        100);
    EXPECT_EQ(ind.fitness, 0);
  }

  for (const auto &ind : popTwo.newInds) {
    EXPECT_EQ(ind.representation->name(), "int");
    EXPECT_EQ(
        static_cast<IntIndividualRep *>(ind.representation.get())->getValue(),
        100);
    EXPECT_EQ(ind.fitness, 0);
  }

  EXPECT_EQ(newPop.size(), newPop.newInds.size());
  EXPECT_EQ(popTwo.size(), popTwo.newInds.size());

  newPop.step();
  popTwo.step();

  newPop.step();

  EXPECT_EQ(newPop.getState()->name(), EvaluateFitnessState::Name);
  EXPECT_EQ(popTwo.getState()->name(), EvaluateFitnessState::Name);

  EXPECT_EQ(newPop.newInds.size(), 0);
  EXPECT_EQ(popTwo.newInds.size(), 0);

  for (auto &ind : newPop) {
    EXPECT_EQ(ind.representation->name(), "int");
    EXPECT_EQ(
        static_cast<IntIndividualRep *>(ind.representation.get())->getValue(),
        100);
    EXPECT_EQ(ind.fitness, 0);
  }

  for (const auto &ind : popTwo) {
    EXPECT_EQ(ind.representation->name(), "int");
    EXPECT_EQ(
        static_cast<IntIndividualRep *>(ind.representation.get())->getValue(),
        100);
    EXPECT_EQ(ind.fitness, 0);
  }
}

TEST(StatsTests, PopulationStats) {
  Context ctx = makeDefaultContext();
  Population pop(std::make_unique<InitialPopState>(ctx));

  pop.currentInds.emplace_back(nullptr, 0);
  pop.currentInds.emplace_back(nullptr, 1);
  pop.currentInds.emplace_back(nullptr, 2);
  pop.currentInds.emplace_back(nullptr, 3);
  pop.currentInds.emplace_back(nullptr, 4);
  pop.currentInds.emplace_back(nullptr, 5);

  PopulationStats stats = pop.getStats();

  EXPECT_EQ(stats.meanFitness, 2.5);
  EXPECT_EQ(stats.minFitness, 0);
  EXPECT_EQ(stats.maxFitness, 5);
  EXPECT_EQ(stats.medianFitness, 2);
  EXPECT_EQ(stats.upperQuartileFitness, 4);
  EXPECT_EQ(stats.lowerQuartileFitness, 1);

  std::stringstream ss;

  ss << stats;

  EXPECT_EQ(ss.str(), "5,0,2.5,2,4,1");
}