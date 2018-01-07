//
// Created by Oliver Downard on 19/12/2017.
//

#include <random>
#include "include/PopulationStates.h"
#include "include/Fitness.h"

const std::string EvaluateFitnessState::Name = "EvaluateFitnessState";
const std::string GeneratePopState::Name = "GeneratePopState";
const std::string VariationState::Name = "VariationState";
const std::string SurvivalState::Name = "SurvivalState";

std::unique_ptr<PopulationState> InitialPopState::execute(Population &pop) {
  return std::make_unique<GeneratePopState>(ctx);
}

std::unique_ptr<PopulationState> GeneratePopState::execute(Population &pop) {
  std::vector<Individual> inds;
  inds.reserve(ctx.popSize);

  for (Context::PopSizeType i = 0; i < ctx.popSize; ++i) {
    inds.push_back(ctx.individualMaker());
  }

  return std::make_unique<EvaluateFitnessState>(ctx);
}

std::unique_ptr<PopulationState> EvaluateFitnessState::execute(Population &pop) {
  // Slightly complex under coevolution as we need other populations to

  // Tell the manager this population wants to be processed, it may run straight
  // away if all other relevant populations are ready.
  // Every time this is run check if the fitness evaluation has been done and
  // change states if it has.

  if (!registeredSeqno.has_value()) {
    registeredSeqno = ctx.fitnessManager->readySignal(&pop);
  }

  auto last = ctx.fitnessManager->lastEvaluation();
  if (last.has_value() && last.value() >= registeredSeqno.value()) {
    return std::make_unique<VariationState>(ctx);
  }

  return nullptr;
}

Individual& VariationState::tournamentSelectInd(Population& p) {
  static std::random_device
      rd; // Will be used to obtain a seed for the random number engine
  static std::mt19937 gen(
      rd()); // Standard mersenne_twister_engine seeded with rd()
  std::uniform_int_distribution<size_t> dis(0, p.size());

  Individual* ind = &p.currentInds[dis(gen)];

  for (size_t i = 1; i < ctx.tournSize; ++i) {
    Individual* test = &p.currentInds[dis(gen)];
    if (test->fitness > ind->fitness) {
      ind = test;
    }
  }

  return *ind;
}

std::unique_ptr<PopulationState> VariationState::execute(Population &pop) {
  for (size_t i = 0; i < pop.size(); ++i) {
    auto ind = ctx.crossoverFunc(tournamentSelectInd(pop), tournamentSelectInd(pop));
    pop.newInds.emplace_back(ctx.mutationFunc(ind));
  }
  return nullptr;
}
