#include <array>
#include <fstream>
#include <include/Context.h>
#include <include/Fitness.h>
#include <include/PopulationStates.h>
#include <iostream>

#include "ExperimentOne.h"
#include "ExperimentTwo.h"

void evolve(size_t numGens, Context ctx) {
  std::vector<size_t> gens;
  std::vector<Population> pops;

  gens.resize(ctx.populationCount);
  std::fill(std::begin(gens), std::end(gens), 0);

  for (size_t i = 0; i < ctx.populationCount; ++i) {
    pops.emplace_back(std::make_unique<InitialPopState>(ctx));
  }

  while (std::find_if(std::begin(gens), std::end(gens), [numGens](size_t x) {
           return x < numGens;
         }) != std::end(gens)) {
    for (size_t i = 0; i < ctx.populationCount; ++i) {
      if (gens.at(i) < numGens) {
        pops.at(i).step();
        if (pops.at(i).getState()->name() == VariationState::Name) {
          ++gens.at(i);
          // std::cout << pops.at(i).getStats() << std::endl;
        }
      }
    }
  }
}

int main(int argc, char *argv[]) {

  std::string resFile = "results.csv";

  if (argc == 2) {
    resFile = argv[1];
  }

  std::ofstream oFile(resFile, std::ios::out);
  oFile << "gen,pop,max,min,mean,median,upper,lower" << std::endl;

  // experimentOne(oFile);
  evolve(600, ExpTwo::setup(oFile));

  oFile.close();
  return 0;
}