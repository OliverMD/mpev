//
// Created by Oliver Downard on 17/12/2017.
//
#pragma once

#include "include/Population.h"

class GeneratePopState : public PopulationState {
public:
  std::unique_ptr<PopulationState> execute(Population &pop) override;
  std::string name() const override { return "GeneratePopState"; }
};

class EvaluateFitnessState : public PopulationState {
public:
  std::unique_ptr<PopulationState> execute(Population &pop) override;
  std::string name() const override { return "EvaluateFitnessState"; }
};

class VariationState : public PopulationState {
public:
  std::unique_ptr<PopulationState> execute(Population &pop) override;
  std::string name() const override { return "VariationState"; }
};