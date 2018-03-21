//
// Created by Oliver Downard on 02/01/2018.
//

#include <include/Context.h>
#include <include/Individual.h>
#include <random>

Individual makeRandomIntIndivdual(Context &ctx) {
  static std::uniform_int_distribution<int> dis(
      std::numeric_limits<int>::min());
  return {std::make_unique<IntIndividualRep>(dis(ctx.rng)), 0};
}

std::vector<Individual> crossoverIntIndividuals(Context &ctx, Individual &b,
                                                Individual &a) {
  std::vector<Individual> ret{};
  ret.emplace_back(std::make_unique<IntIndividualRep>(
          dynamic_cast<IntIndividualRep*>(a.representation.get())->getValue() +
          dynamic_cast<IntIndividualRep*>(b.representation.get())->getValue()),
      0);
  return ret;
}

Individual mutateIntIndividual(Context &ctx, Individual &a) {
  static std::uniform_int_distribution<int> dis(-10, 10);
  return {
      std::make_unique<IntIndividualRep>(
          dis(ctx.rng) +
          dynamic_cast<IntIndividualRep *>(a.representation.get())->getValue()),
      0};
}