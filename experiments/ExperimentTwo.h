//
// Created by Oliver Downard on 15/02/2018.
//

#pragma once

#include "OnesInd.h"
#include "include/Selectors.h"

#include <cmath>

namespace ExpTwo {

constexpr size_t NumBits = 10;
constexpr float CrossoverProb = 0;
constexpr float MutationProb = 0.05;
constexpr size_t Dimensions = 10;

const std::string name = "ExpTwo";

using Rep = OnesIndRep<NumBits, Dimensions>;

Individual makeOnesInd() { return {std::make_unique<Rep>(), 0}; }

float fitnessFunc(const IndividualRep *a, const IndividualRep *b) {
  const Rep *aa = dynamic_cast<const Rep *>(a);
  const Rep *bb = dynamic_cast<const Rep *>(b);

  size_t dim = 0;
  size_t largestDiff =
      std::abs(static_cast<long>(aa->getNumOnes(0) - bb->getNumOnes(0)));
  for (size_t d = 1; d < Dimensions; ++d) {
    const size_t diff =
        std::abs(static_cast<long>(aa->getNumOnes(d) - bb->getNumOnes(d)));
    if (diff > largestDiff) {
      largestDiff = diff;
      dim = d;
    }
  }

  return aa->getNumOnes(dim) > bb->getNumOnes(dim) ? 1.0 : 0.0;
}

std::vector<Individual> crossOnesInds(Individual &a, Individual &b) {
  std::vector<Individual> ret{};
  Rep *aa = dynamic_cast<Rep *>(a.representation.get());
  ret.emplace_back(std::make_unique<Rep>(*aa), 0);
  return ret;
}

Individual mutateOnesInd(Individual &a) {
  static std::random_device
      rd; // Will be used to obtain a seed for the random number engine
  static std::mt19937 gen(
      rd()); // Standard mersenne_twister_engine seeded with rd()
  static std::uniform_real_distribution<float> dis{};
  static std::uniform_int_distribution<bool> valDis{};
  Rep *aa = dynamic_cast<Rep *>(a.representation.get());
  std::array<std::vector<bool>, Dimensions> ret{};

  for (size_t d = 0; d < Dimensions; ++d) {
    std::vector<bool> r = aa->getSlice(d, 0, NumBits);
    for (size_t i = 0; i < r.size(); ++i) {
      float z = dis(gen);
      if (z < MutationProb) {
        r[i] = valDis(gen);
      }
    }
    ret[d] = r;
  }
  return {std::make_unique<Rep>(ret), 0};
}

Context setup(std::ofstream &out, std::ofstream &sOut) {
  constexpr size_t popCount = 2;
  Context ctx = makeDefaultContext();
  ctx.tournSize = 5;
  ctx.mutationFunc = mutateOnesInd;
  ctx.crossoverFunc = crossOnesInds;
  ctx.individualMaker = makeOnesInd;

  ctx.varySelectorCreator = createRouletteSelect;
  ctx.survivalSelectorCreator = createRouletteSelect;

  ctx.objectiveFunc = [](const IndividualRep *rep) {
    const Rep *a = dynamic_cast<const Rep *>(rep);
    float total = 0;
    for (size_t d = 0; d < Dimensions; ++d) {
      total += a->getNumOnes(d);
    }
    return total;
  };

  ctx.popSize = 25;

  ctx.fitnessManager = std::make_unique<
      CoevFitnessManager<DefaultFitnessEv<ExpTwo::fitnessFunc>>>(popCount, 15);
  ctx.populationCount = popCount;

  ctx.objectiveReportCallback = [&out](PopulationStats stats, uint32_t popId,
                                       size_t gen) {
    out << gen << "," << popId << "," << stats << std::endl;
  };

  ctx.subjectiveReportCallback = [&sOut](PopulationStats stats, uint32_t popId,
                                         size_t gen) {
    sOut << gen << "," << popId << "," << stats << std::endl;
  };
  return ctx;
}

} // namespace ExpTwo
