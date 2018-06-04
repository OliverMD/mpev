#include <array>
#include <cpptoml.h>
#include <experimental/filesystem>
#include <fstream>
#include <include/Context.h>
#include <include/Fitness.h>
#include <include/PopulationStates.h>
#include <iostream>
#include <thread>

#include "ExperimentOne.h"
#include "ExperimentTwo.h"
#include "ExperimentThree.h"

namespace fs = std::experimental::filesystem;

using ExperimentGen = std::function<Context(std::ofstream &, std::ofstream &,
                                            std::ofstream &, unsigned int,
                                            size_t, size_t, size_t)>;

const std::unordered_map<std::string, ExperimentGen> setups = {
    {ExpOne::name, ExpOne::setup},
    {ExpTwo::name, ExpTwo::setup},
    {ExpThree::name, ExpThree::setup}};

struct ExperimentConfig {
  std::string game;
  ExperimentGen ctxGen;
  uint numRuns;
  std::string desc;
  size_t numPops;
  size_t numInds;
  size_t numCompetitions;
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

void runExperiment(ExperimentConfig exp, fs::path resPath) {
  fs::path thisResPath = resPath;
  thisResPath.append(exp.game);
  fs::create_directories(thisResPath);
  std::vector<unsigned int> seeds;

  for (uint i = 0; i < exp.numRuns; ++i) {
    const std::string obFilename =
        std::string{"ob_results_"} + std::to_string(i) + std::string{".csv"};
    const std::string subFilename =
        std::string{"sub_results_"} + std::to_string(i) + std::string{".csv"};
    const std::string indFilename =
        std::string{"ind_reps_"} + std::to_string(i) + std::string{".csv"};

    std::ofstream oFile(thisResPath.append(obFilename), std::ios::out);
    thisResPath.remove_filename();
    std::ofstream sFile{thisResPath.append(subFilename), std::ios::out};
    thisResPath.remove_filename();
    std::ofstream iFile{thisResPath.append(indFilename), std::ios::out};
    thisResPath.remove_filename();

    oFile << "gen,pop,max,min,mean,median,upper,lower" << std::endl;
    sFile << "gen,pop,max,min,mean,median,upper,lower" << std::endl;
    iFile << "gen,pop,individual" << std::endl;

    unsigned int seed = std::random_device()();
    seeds.push_back(seed);
    evolve(600, exp.ctxGen(oFile, sFile, iFile, seed, exp.numPops, exp.numInds, exp.numCompetitions));

    oFile.close();
    sFile.close();
    iFile.close();
  }
  std::ofstream readmeFile{thisResPath.append("readme.txt"), std::ios::out};
  readmeFile << exp.game << std::endl << std::endl;
  readmeFile << "numRuns=" << exp.numRuns << std::endl;
  readmeFile << exp.desc << std::endl;

  readmeFile << "seeds=";
  for (auto seed = std::begin(seeds); seed != std::end(seeds); ++seed) {
    if (seed == std::begin(seeds)) {
      readmeFile << *seed;
    } else {
      readmeFile << "," << *seed;
    }
  }
  readmeFile << std::endl;

  readmeFile.close();
}

void runFromConfig(RunConfig cfg) {
  const auto resPath = fs::path{cfg.rootResultsLoc};

  std::vector<std::thread> threads;
  for (const auto &exp : cfg.experiments) {
    std::thread nt(runExperiment, exp, resPath);
    threads.push_back(std::move(nt));
  }

  for (auto& th: threads) {
    th.join();
  }
}

RunConfig parseTomlConfig(fs::path configFile) {
  auto config = cpptoml::parse_file(configFile.string());

  RunConfig rConfig{};

  auto loc = config->get_as<std::string>("rootResultsLoc");
  if (!loc) {
    throw std::runtime_error{"result location not found in config file"};
  }
  rConfig.rootResultsLoc = *loc;

  auto tarr = config->get_table_array("experiments");

  if (!tarr) {
    throw std::runtime_error{"experiment array not found in config file"};
  }

  for (const auto &table : *tarr) {
    ExperimentConfig eConfig{};

    auto game = table->get_as<std::string>("game");
    if (!game) {
      throw std::runtime_error{"experiment has no game"};
    }
    eConfig.game = *game;
    eConfig.ctxGen = setups.at(*game);

    auto numRuns = table->get_as<uint>("runs");
    if (!numRuns) {
      throw std::runtime_error{"experiment has no runs"};
    }
    eConfig.numRuns = *numRuns;

    auto desc = table->get_as<std::string>("description");
    if (desc) {
      eConfig.desc = *desc;
    } else {
      eConfig.desc = "";
    }

    auto numPops = table->get_as<uint>("numPops");
    if (numPops) {
      eConfig.numPops = *numPops;
    } else {
      throw std::runtime_error{"numPops must be specified"};
    }

    auto numInds = table->get_as<uint>("numInds");
    if (numInds) {
      eConfig.numInds = *numInds;
    } else {
      throw std::runtime_error{"numInds must be specified"};
    }

    auto numComps = table->get_as<uint>("numComps");
    if (numComps) {
      eConfig.numCompetitions = *numComps;
    } else {
      throw std::runtime_error{"numComps must be specified"};
    }

    rConfig.experiments.push_back(eConfig);
  }
  return rConfig;
}

int main(int argc, char *argv[]) {

  if (argc != 2) {
    std::cout << "Invalid arguments" << std::endl;
    std::cout << "Example: ./experiments path/to/config.toml" << std::endl;
    return 1;
  }

  std::cout << "Waiting for input..." << std::endl;
  std::string in;
  std::cin >> in;
  std::cout << "Input received! Starting..." << std::endl;

  RunConfig runConfig;
  try {
    runConfig = parseTomlConfig(argv[1]);
  } catch (std::runtime_error& e) {
    std::cout << "Error parsing config: " << e.what() << std::endl;
    return 1;
  }
  runFromConfig(runConfig);

  return 0;
}