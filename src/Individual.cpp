//
// Created by Oliver Downard on 02/01/2018.
//

#include <include/Individual.h>
#include <random>

Individual makeRandomIntIndivdual() {
  static std::random_device
      rd; // Will be used to obtain a seed for the random number engine
  static std::mt19937 gen(
      rd()); // Standard mersenne_twister_engine seeded with rd()
  static std::uniform_int_distribution<int> dis(
      std::numeric_limits<int>::min());
  return {std::make_unique<IntIndividualRep>(dis(gen)), 0};
}

Individual crossoverIntIndividuals(Individual &a, Individual &b) {
  return {std::make_unique<IntIndividualRep>(
          dynamic_cast<IntIndividualRep*>(a.representation.get())->getValue() +
          dynamic_cast<IntIndividualRep*>(b.representation.get())->getValue()),
      0};
}

Individual mutateIntIndividual(Individual &a) {
  static std::random_device
      rd; // Will be used to obtain a seed for the random number engine
  static std::mt19937 gen(
      rd()); // Standard mersenne_twister_engine seeded with rd()
  static std::uniform_int_distribution<int> dis(-10, 10);

  return {
      std::make_unique<IntIndividualRep>(
          dis(gen) +
          dynamic_cast<IntIndividualRep *>(a.representation.get())->getValue()),
      0};
}