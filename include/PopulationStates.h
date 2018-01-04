//
// Created by Oliver Downard on 17/12/2017.
//
#pragma once

#include "include/Population.h"

class GeneratePopState : public PopulationState {
public:
  GeneratePopState(Context& ctx) : ctx{ctx} {}
  std::unique_ptr<PopulationState> execute(Population &pop) override;
  std::string name() const override { return "GeneratePopState"; }
private:
  Context& ctx;
};

class EvaluateFitnessState : public PopulationState {
public:
  static const std::string Name;
  std::unique_ptr<PopulationState> execute(Population &pop) override;
  std::string name() const override { return Name; }
};

class VariationState : public PopulationState {
public:
  std::unique_ptr<PopulationState> execute(Population &pop) override;
  std::string name() const override { return "VariationState"; }
};