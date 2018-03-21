//
// Created by Oliver Downard on 31/01/2018.
//

#include <random>
#include "include/Selectors.h"

Individual& TournamentSelect::operator()(std::vector<Individual> &p) {
  std::uniform_int_distribution<size_t> dis(0, p.size() - 1);

  Individual *ind = &(p.at(dis(ctx.rng)));

  for (size_t i = 1; i < ctx.tournSize; ++i) {
    Individual *test = &p[dis(ctx.rng)];
    if (test->fitness > ind->fitness) {
      ind = test;
    }
  }

  return *ind;
}

Individual& RouletteWheelSelect::operator()(Context::OldPop &old, Context::NewPop &newPop) {
  std::uniform_real_distribution<double> dis(0, 1);
  if (inds.size() == 0) {
    for (Individual &i : old) {
      inds.insert(std::upper_bound(std::begin(inds), std::end(inds), &i,
                                   [](const Individual *a,
                                      const Individual *b) -> bool {
                                     return a->fitness > b->fitness;
                                   }), &i);
      fitSum += i.fitness;
    }
    for (Individual &i : newPop) {
      inds.insert(std::upper_bound(std::begin(inds), std::end(inds), &i,
                                   [](const Individual *a,
                                      const Individual *b) -> bool {
                                     return a->fitness > b->fitness;
                                   }), &i);
      fitSum += i.fitness;
    }
  }

  double target = dis(ctx.rng) * fitSum;
  double total{0};

  for (Individual* ind : inds) {
    total += ind->fitness;
    if (target <= total) {
      return *ind;
    }
  }
  return **std::rbegin(inds);
}

Individual& StraightCopySelector::operator()(std::vector<Individual> &p) {
  if (!init) {
    iter = std::begin(p);
    init = true;
  }
  return *(iter++);
}

Individual& TopSelector::operator()(Context::OldPop &old,
                                    Context::NewPop &newPop) {
  if (inds.size() == 0) {
    for (Individual &i : old) {
      inds.insert(std::upper_bound(std::begin(inds), std::end(inds), &i,
                                   [](const Individual *a,
                                      const Individual *b) -> bool {
                                     return a->fitness > b->fitness;
                                   }), &i);
    }
    for (Individual &i : newPop) {
      inds.insert(std::upper_bound(std::begin(inds), std::end(inds), &i,
                                   [](const Individual *a,
                                      const Individual *b) -> bool {
                                     return a->fitness > b->fitness;
                                   }), &i);
    }
    iter = std::begin(inds);
  }
  return **(iter++);
}

TournamentSelect createTournSelect(Context &ctx) {
  return TournamentSelect{ctx};
}

RouletteWheelSelect createRouletteSelect(Context &ctx) {
  return RouletteWheelSelect{ctx};
}

StraightCopySelector createStraightCopy(Context &) {
  return StraightCopySelector{};
}

TopSelector createTopSelector(Context&) {
  return TopSelector{};
}