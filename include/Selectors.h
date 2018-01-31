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
private:
  Context& ctx;
};

class RouletteWheelSelect {
public:
  Individual &operator()(std::vector<Individual>& p);
private:
  std::vector<Individual*> inds;
  double fitSum;
};

class StraightCopySelector {
public:
  StraightCopySelector() : init{false} {}
  Individual& operator()(std::vector<Individual>& p);
private:
  std::vector<Individual>::iterator iter;
  bool init;
};

Context::Selector createTournSelect(Context& ctx);

Context::Selector createRouletteSelect(Context& ctx);

Context::Selector createStraightCopy(Context&);
