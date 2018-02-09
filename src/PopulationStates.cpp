//
// Created by Oliver Downard on 19/12/2017.
//

#include "include/PopulationStates.h"
#include "include/Fitness.h"

const std::string EvaluateFitnessState::Name = "EvaluateFitnessState";
const std::string GeneratePopState::Name = "GeneratePopState";
const std::string VariationState::Name = "VariationState";
const std::string ReporterState::Name = "ReporterState";
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

  pop.replacePopulation(std::move(inds));
  return std::make_unique<EvaluateFitnessState>(ctx);
}

std::unique_ptr<PopulationState>
EvaluateFitnessState::execute(Population &pop) {
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
    return std::make_unique<ReporterState>(ctx);
  }

  return nullptr;
}

std::unique_ptr<PopulationState> ReporterState::execute(Population &pop) {
  if (ctx.reporterCallback == nullptr) {
    return std::make_unique<VariationState>(ctx);
  }
  std::vector<float> fits;
  fits.reserve(pop.size());
  for (const auto& ind : pop.currentInds) {
    fits.emplace_back(ctx.objectiveFunc(ind.representation.get()));
  }
  ctx.reporterCallback(Population::calculateFitnessStats(fits), pop.getId(), pop.age);
  ++pop.age;
  return std::make_unique<VariationState>(ctx);
}

std::unique_ptr<PopulationState> VariationState::execute(Population &pop) {
  pop.newInds.reserve(pop.size());
  auto selector = ctx.varySelectorCreator(ctx);
  for (size_t i = 0; i < pop.size(); ++i) {
    auto inds =
        ctx.crossoverFunc(selector(pop.currentInds), selector(pop.currentInds));
    for (auto &ind : inds) {
      pop.newInds.emplace_back(ctx.mutationFunc(ind));
    }
  }
  return std::make_unique<SurvivalState>(ctx);
}

std::unique_ptr<PopulationState> SurvivalState::execute(Population &pop) {
  auto selector = ctx.survivalSelectorCreator(ctx);
  size_t size = pop.size();
  pop.currentInds.clear();
  for (size_t i = 0; i < size; ++i) {
    auto &ind = selector(pop.currentInds, pop.newInds);
    pop.currentInds.emplace_back(ind.representation->copy(), 0);
  }
  pop.newInds.clear();
  return std::make_unique<EvaluateFitnessState>(ctx);
}
