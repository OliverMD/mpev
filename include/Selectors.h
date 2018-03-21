//
// Created by Oliver Downard on 31/01/2018.
//
#pragma once

#include "Individual.h"
#include "Population.h"

class TournamentSelect {
public:
  TournamentSelect(Context& ctx) : ctx{ctx} {}
  Individual &operator()(std::vector<Individual>& p);
  Individual &operator()(Context::OldPop &old, Context::NewPop &newPop) {
    return this->operator()(newPop);
  }

private:
  Context& ctx;
};

class RouletteWheelSelect {
public:
  RouletteWheelSelect(Context &ctx) : ctx{ctx} {}
  Individual &operator()(std::vector<Individual>& p) {
    // TODO: Tech Debt - Remove this workaround
    std::vector<Individual> empty;
    return this->operator()(p, empty);
  }
  Individual &operator()(Context::OldPop &old, Context::NewPop &newPop);

private:
  std::vector<Individual*> inds;
  double fitSum;
  Context &ctx;
};

class StraightCopySelector {
public:
  StraightCopySelector() : init{false} {}
  Individual& operator()(std::vector<Individual>& p);
  Individual &operator()(Context::OldPop &old, Context::NewPop &newPop) {
    return this->operator()(newPop);
  }

private:
  std::vector<Individual>::iterator iter;
  bool init;
};

class TopSelector {
public:
  Individual& operator()(Context::OldPop& old, Context::NewPop& newPop);
private:
  std::vector<Individual*> inds;
  std::vector<Individual*>::iterator iter;
};

TournamentSelect createTournSelect(Context &ctx);

RouletteWheelSelect createRouletteSelect(Context &ctx);

StraightCopySelector createStraightCopy(Context &);

TopSelector createTopSelector(Context &);
