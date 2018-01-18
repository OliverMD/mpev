//
// Created by Oliver Downard on 17/12/2017.
//
#pragma once

#include "include/Population.h"
#include <optional>
#include <random>

template <typename FitEv> class EvaluateFitnessState;

template <typename FitEv> class VariationState;

template <typename FitEv> class SurvivalState;

template <typename FitEv>
class GeneratePopState : public PopulationState<FitEv> {
public:
  static const std::string Name;
  GeneratePopState(Context<FitEv> &ctx) : ctx{ctx} {}
  std::unique_ptr<PopulationState<FitEv>>
  execute(Population<FitEv> &pop) override {
    std::vector<Individual> inds;
    inds.reserve(ctx.popSize);

    for (typename Context<FitEv>::PopSizeType i = 0; i < ctx.popSize; ++i) {
      inds.push_back(ctx.individualMaker());
    }

    return std::make_unique<EvaluateFitnessState<FitEv>>(ctx);
  };
  std::string name() const override { return Name; }

private:
  Context<FitEv> &ctx;
};

template <typename T>
const std::string GeneratePopState<T>::Name = "GeneratePopState";

template <typename FitEv>
class EvaluateFitnessState : public PopulationState<FitEv> {
public:
  static const std::string Name;
  EvaluateFitnessState(Context<FitEv> &ctx) : ctx{ctx} {}
  std::unique_ptr<PopulationState<FitEv>>
  execute(Population<FitEv> &pop) override {
    // Slightly complex under coevolution as we need other populations to

    // Tell the manager this population wants to be processed, it may run
    // straight away if all other relevant populations are ready. Every time
    // this is run check if the fitness evaluation has been done and change
    // states if it has.

    if (!registeredSeqno.has_value()) {
      registeredSeqno = ctx.fitnessManager->readySignal(&pop);
    }

    auto last = ctx.fitnessManager->lastEvaluation();
    if (last.has_value() && last.value() >= registeredSeqno.value()) {
      return std::make_unique<VariationState<FitEv>>(ctx);
    }

    return nullptr;
  }
  std::string name() const override { return Name; }

private:
  Context<FitEv> &ctx;
  std::optional<uint32_t> registeredSeqno;
};

template <typename T>
const std::string EvaluateFitnessState<T>::Name = "EvaluateFitnessState";

template <typename FitEv> class VariationState : public PopulationState<FitEv> {
public:
  static const std::string Name;
  VariationState(Context<FitEv> &ctx) : ctx{ctx} {}
  std::unique_ptr<PopulationState<FitEv>>
  execute(Population<FitEv> &pop) override {
    pop.newInds.reserve(pop.size());
    for (size_t i = 0; i < pop.size(); ++i) {
      auto ind =
          ctx.crossoverFunc(tournamentSelectInd(pop), tournamentSelectInd(pop));
      pop.newInds.emplace_back(ctx.mutationFunc(ind));
    }
    return std::make_unique<SurvivalState<FitEv>>(ctx);
  }
  std::string name() const override { return Name; }

private:
  Context<FitEv> &ctx;
  Individual &tournamentSelectInd(Population<FitEv> &p) {
    static std::random_device
        rd; // Will be used to obtain a seed for the random number engine
    static std::mt19937 gen(
        rd()); // Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<size_t> dis(0, p.size());

    Individual *ind = &p.currentInds[dis(gen)];

    for (size_t i = 1; i < ctx.tournSize; ++i) {
      Individual *test = &p.currentInds[dis(gen)];
      if (test->fitness > ind->fitness) {
        ind = test;
      }
    }

    return *ind;
  }
};

template <typename T>
const std::string VariationState<T>::Name = "VariationState";

template <typename FitEv> class SurvivalState : public PopulationState<FitEv> {
public:
  static const std::string Name;
  SurvivalState(Context<FitEv> &ctx) : ctx{ctx} {}
  std::unique_ptr<PopulationState<FitEv>>
  execute(Population<FitEv> &pop) override {
    pop.replacePopulation(std::move(pop.newInds));
    pop.newInds.clear();
    return std::make_unique<EvaluateFitnessState<FitEv>>(ctx);
  }
  std::string name() const override { return Name; }

private:
  Context<FitEv> &ctx;
};

template <typename T>
const std::string SurvivalState<T>::Name = "SurvivalState";