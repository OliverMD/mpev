//
// Created by Oliver Downard on 17/12/2017.
//
#pragma once

#include <optional>
#include "include/Population.h"

class GeneratePopState : public PopulationState {
public:
  static const std::string Name;
  GeneratePopState(Context& ctx) : ctx{ctx} {}
  std::unique_ptr<PopulationState> execute(Population &pop) override;
  std::string name() const override { return Name; }
private:
  Context& ctx;
};

class EvaluateFitnessState : public PopulationState {
public:
  static const std::string Name;
  EvaluateFitnessState(Context& ctx) : ctx{ctx} {}
  std::unique_ptr<PopulationState> execute(Population &pop) override;
  std::string name() const override { return Name; }
private:
  Context& ctx;
  std::optional<uint32_t> registeredSeqno;
};

class ReporterState : public PopulationState {
public:
  static const std::string Name;
  ReporterState(Context& ctx) : ctx{ctx} {}
  std::unique_ptr<PopulationState> execute(Population& pop) override;
  std::string name() const override { return Name; }
private:
  Context& ctx;
};

class VariationState : public PopulationState {
public:
  static const std::string Name;
  VariationState(Context& ctx) : ctx{ctx} {}
  std::unique_ptr<PopulationState> execute(Population &pop) override;
  std::string name() const override { return Name; }
private:
  Context& ctx;
};

class SurvivalState : public PopulationState {
public:
  static const std::string Name;
  SurvivalState(Context& ctx) : ctx{ctx} {}
  std::unique_ptr<PopulationState> execute(Population &pop) override;
  std::string name() const override { return Name; }
private:
  Context& ctx;
  std::optional<uint32_t> registeredSeqno;
};