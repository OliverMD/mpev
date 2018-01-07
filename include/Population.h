//
// Created by Oliver Downard on 17/12/2017.
//

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "include/Individual.h"
#include "Context.h"

class Population;
class FitnessManager;
//class FitnessManager::iterator;
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
  InitialPopState(Context& ctx) : ctx{ctx} {}
  std::unique_ptr<PopulationState> execute(Population &pop) override;
  std::string name() const override { return "InitialPopState"; }
private:
  Context& ctx;
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

  auto cbegin() const {
    return std::cbegin(currentInds);
  };

  auto cend() const {
    return std::cend(currentInds);
  }

  auto begin() {
    return std::begin(currentInds);
  }

  auto end() {
    return std::end(currentInds);
  }

  size_t size() {
    return currentInds.size();
  }

  uint32_t getId() { return id; }

  std::vector<Individual> newInds;
  std::vector<Individual> currentInds;

private:
  static std::atomic<uint32_t> CURRENTID;
  const uint32_t id;
  std::unique_ptr<PopulationState> state;
};
