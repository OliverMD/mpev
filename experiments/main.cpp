#include <array>
#include <include/Context.h>
#include <include/Fitness.h>
#include <include/PopulationStates.h>
#include <iostream>
#include <fstream>

#include "include/Selectors.h"

#include "ExperimentOne.h"
#include "OnesInd.h"

void evolve(size_t numGens, Context ctx, size_t numPops) {
  std::vector<size_t> gens;
  std::vector<Population> pops;

  gens.resize(numPops);
  std::fill(std::begin(gens), std::end(gens), 0);

  for (size_t i = 0; i < numPops; ++i) {
    pops.emplace_back(std::make_unique<InitialPopState>(ctx));
  }

  while (std::find_if(std::begin(gens), std::end(gens), [numGens](size_t x){
    return x < numGens;
  }) != std::end(gens)) {
    for (size_t i = 0; i < numPops; ++i) {
      if (gens.at(i) < numGens) {
        pops.at(i).step();
        if (pops.at(i).getState()->name() == VariationState::Name) {
          ++gens.at(i);
          //std::cout << pops.at(i).getStats() << std::endl;
        }
      }
    }
  }
}

int main(int argc, char* argv[]) {

  std::string resFile = "results.csv";

  if (argc == 2) {
    resFile = argv[1];
  }

  std::ofstream oFile(resFile, std::ios::out);
  oFile << "gen,pop,max,min,mean,median,upper,lower" << std::endl;

  // experimentOne(oFile);
  evolve(600, ExpOne::setup(oFile), 2);

  oFile.close();
  return 0;
}