//
// Created by Oliver Downard on 17/12/2017.
//

#pragma once

#include <memory>
#include <numeric>
#include <string>
#include <vector>

#include "Context.h"
#include "Individual.h"
#include "Stats.h"

class Population;

/*
 * Interface for population states
 * May need to become an abstract class in the future
 */
class PopulationState {
public:
  /**
   * void execute(Population& p)
   * execute the state.
   *
   * @return The new state, nullptr if no change
   */
  virtual std::unique_ptr<PopulationState> execute(Population &pop) = 0;

  /**
   *
   * @return the name of the state
   */
  virtual std::string name() const = 0;
};

class InitialPopState : public PopulationState {
public:
  InitialPopState(Context &ctx) : ctx{ctx} {}
  std::unique_ptr<PopulationState> execute(Population &pop) override;
  std::string name() const override { return "InitialPopState"; }

private:
  Context &ctx;
};

class Population {
public:
  Population(std::unique_ptr<PopulationState> startState)
      : id{CURRENTID++}, state{std::move(startState)} {}

  void step() {
    auto newState = state->execute(*this);
    if (newState != nullptr) {
      state = std::move(newState);
    }
  }
  const PopulationState *getState() const { return state.get(); }

  void replacePopulation(std::vector<Individual> newPop) {
    currentInds.swap(newPop);
  }

  PopulationStats getStats() const {
    PopulationStats ret{};

    std::vector<float> fits;
    fits.reserve(size());

    for (const auto &ind : currentInds) {
      fits.emplace_back(ind.fitness);
    }

    std::sort(std::begin(fits), std::end(fits));
    ret.medianFitness = fits[(size() - 1) / 2];
    ret.lowerQuartileFitness = fits[(size() - 1) / 4];
    ret.upperQuartileFitness = fits[size() - 1 - ((size() - 1)/ 4)];
    ret.maxFitness = fits[size() - 1];
    ret.minFitness = fits[0];
    ret.meanFitness =
        std::accumulate(std::begin(fits), std::end(fits), (float)0.0) / (float)size();

    return ret;
  }

  auto cbegin() const { return std::cbegin(currentInds); };

  auto cend() const { return std::cend(currentInds); }

  auto begin() { return std::begin(currentInds); }

  auto end() { return std::end(currentInds); }

  size_t size() const { return currentInds.size(); }

  uint32_t getId() const { return id; }

  std::vector<Individual> newInds;
  std::vector<Individual> currentInds;

private:
  static std::atomic<uint32_t> CURRENTID;
  const uint32_t id;
  std::unique_ptr<PopulationState> state;
};
