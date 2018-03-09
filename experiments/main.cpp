#include <array>
#include <experimental/filesystem>
#include <fstream>
#include <include/Context.h>
#include <include/Fitness.h>
#include <include/PopulationStates.h>
#include <iostream>

#include "ExperimentOne.h"
#include "ExperimentTwo.h"
#include "ExperimentThree.h"

namespace fs = std::experimental::filesystem;

using ExperimentGen = std::function<Context(std::ofstream &, std::ofstream &)>;

struct ExperimentConfig {
  std::string name;
  ExperimentGen ctxGen;
  uint numRuns;
  std::string desc;
};

struct RunConfig {
  std::string rootResultsLoc;
  std::vector<ExperimentConfig> experiments;
};

void evolve(size_t numGens, Context ctx) {
  std::vector<size_t> gens;
  std::vector<Population> pops;

  gens.resize(ctx.populationCount);
  std::fill(std::begin(gens), std::end(gens), 0);

  std::unordered_map<uint32_t, std::vector<uint32_t>> actualMap;

  for (size_t i = 0; i < ctx.populationCount; ++i) {
    pops.emplace_back(std::make_unique<InitialPopState>(ctx));
  }

  if (ctx.provisionalMap.size() > 0) {
    // pops is vector so will still be in order
    for (size_t i = 0; i < ctx.populationCount; ++i) {
      actualMap[pops.at(i).getId()] = {};
      for (auto v : ctx.provisionalMap[i]) {
        actualMap[pops.at(i).getId()].push_back(pops.at(v).getId());
      }
    }
  }

  ctx.fitnessManager->setCompMap(actualMap);

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

void runFromConfig(RunConfig cfg) {
  const auto resPath = fs::path{cfg.rootResultsLoc};

  for (const auto &exp : cfg.experiments) {
    fs::path thisResPath = resPath;
    thisResPath.append(exp.name);
    fs::create_directories(thisResPath);

    for (uint i = 0; i < exp.numRuns; ++i) {
      const std::string obFilename =
          std::string{"ob_results_"} + std::to_string(i) + std::string{".csv"};
      const std::string subFilename =
          std::string{"sub_results_"} + std::to_string(i) + std::string{".csv"};

      std::ofstream oFile(thisResPath.append(obFilename), std::ios::out);
      thisResPath.remove_filename();
      std::ofstream sFile{thisResPath.append(subFilename), std::ios::out};

      thisResPath.remove_filename();

      oFile << "gen,pop,max,min,mean,median,upper,lower" << std::endl;
      sFile << "gen,pop,max,min,mean,median,upper,lower" << std::endl;

      evolve(600, exp.ctxGen(oFile, sFile));

      oFile.close();
      sFile.close();
    }
    std::ofstream readmeFile{thisResPath.append("readme.txt"), std::ios::out};
    readmeFile << exp.name << std::endl << std::endl;
    readmeFile << "numRuns=" << exp.numRuns << std::endl;
    readmeFile << exp.desc << std::endl;

    readmeFile.close();
  }
}

int main(int argc, char *argv[]) {
  RunConfig runConfig{"./results/", {{"ExpThree", ExpThree::setup, 2, ""}}};

  runFromConfig(runConfig);

  return 0;
}