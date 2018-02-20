#include <array>
#include <fstream>
#include <include/Context.h>
#include <include/Fitness.h>
#include <include/PopulationStates.h>
#include <iostream>

#include "ExperimentOne.h"
#include "ExperimentTwo.h"
#include "ExperimentThree.h"

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
        }
      }
    }
  }
}

void iterExp(std::function<Context(std::ofstream &)> ctxGen, size_t count) {
  for (uint i = 0; i < count; ++i) {
    std::string filename =
        std::string{"ob_results_"} + std::to_string(i) + std::string{".csv"};

    std::ofstream oFile(filename, std::ios::out);
    oFile << "gen,pop,max,min,mean,median,upper,lower" << std::endl;

    evolve(600, ctxGen(oFile));

    oFile.close();
  }
}

int main(int argc, char *argv[]) {

  iterExp(ExpThree::setup, 2);

  return 0;
}