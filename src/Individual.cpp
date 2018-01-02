//
// Created by Oliver Downard on 02/01/2018.
//

#include <include/Individual.h>
#include <random>

Individual makeRandomIntIndivdual() {
  static std::random_device rd;  //Will be used to obtain a seed for the random number engine
  static std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
  static std::uniform_int_distribution<int> dis(std::numeric_limits<int>::min());
  return {std::make_unique<IntIndividualRep>(dis(gen)), 0};
}