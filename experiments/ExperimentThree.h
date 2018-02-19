//
// Created by Oliver Downard on 16/02/2018.
//

#pragma once

#include "ExperimentTwo.h"

namespace ExpThree {
using Rep = ExpTwo::Rep;
float fitnessFunc(const IndividualRep *a, const IndividualRep *b) {
  const Rep *aa = dynamic_cast<const Rep *>(a);
  const Rep *bb = dynamic_cast<const Rep *>(b);

  size_t dim = 0;
  size_t smallestDiff =
      std::abs(static_cast<long>(aa->getNumOnes(0) - bb->getNumOnes(0)));
  for (size_t d = 1; d < ExpTwo::Dimensions; ++d) {
    const size_t diff =
        std::abs(static_cast<long>(aa->getNumOnes(d) - bb->getNumOnes(d)));
    if (diff < smallestDiff) {
      smallestDiff = diff;
      dim = d;
    }
  }

  return aa->getNumOnes(dim) > bb->getNumOnes(dim) ? 1.0 : 0.0;
}

Context setup(std::ofstream &out) {
  constexpr size_t popCount = 10;
  Context ctx = makeDefaultContext();
  ctx.tournSize = 5;
  ctx.mutationFunc = ExpTwo::mutateOnesInd;
  ctx.crossoverFunc = ExpTwo::crossOnesInds;
  ctx.individualMaker = ExpTwo::makeOnesInd;

  ctx.varySelectorCreator = createRouletteSelect;
  ctx.survivalSelectorCreator = createTopSelector;

  ctx.objectiveFunc = [](const IndividualRep *rep) {
    const Rep *a = dynamic_cast<const Rep *>(rep);
    float total = 0;
    for (size_t d = 0; d < ExpTwo::Dimensions; ++d) {
      total += a->getNumOnes(d);
    }
    return total;
  };

  ctx.popSize = 5;

  ctx.fitnessManager = std::make_unique<
      CoevFitnessManager<DefaultFitnessEv<ExpThree::fitnessFunc>>>(popCount, 15);
  ctx.populationCount = popCount;

  ctx.reporterCallback = [&out](PopulationStats stats, uint32_t popId,
                                size_t gen) {
    out << gen << "," << popId << "," << stats << std::endl;
  };
  return ctx;
}

}
