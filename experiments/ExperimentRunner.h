//
// Created by Oliver Downard on 26/07/2018.
//

#ifndef MPEV_EXPERIMENTRUNNER_H
#define MPEV_EXPERIMENTRUNNER_H

#include <include/Context.h>

#include <experimental/filesystem>
#include <folly/futures/Future.h>
#include <fstream>
#include <unordered_map>

namespace fs = std::experimental::filesystem;

using ExperimentGen = std::function<void(Context &, size_t)>;

//const std::unordered_map<std::string, ExperimentGen> setups;

struct ExperimentConfig {
  std::string game;
  std::string name;
  ExperimentGen ctxGen;
  uint numRuns;
  std::string desc;
  size_t numPops;
  size_t numInds;
  size_t numCompetitions;
  std::vector<std::vector<uint32_t>> compMap;
  Context createContext(std::ofstream &out, std::ofstream &sOut,
                        std::ofstream &iOut, unsigned int seed) {
    Context ctx = makeDefaultContext(seed);
    ctx.populationCount = numPops;

    ctx.tournSize = 5;
    ctx.popSize = numInds;
    ctx.provisionalMap = compMap;

    ctx.objectiveReportCallback = [&out](PopulationStats stats, uint32_t popId,
                                         size_t gen) {
      out << gen << "," << popId << "," << stats << std::endl;
    };

    ctx.subjectiveReportCallback = [&sOut](PopulationStats stats,
                                           uint32_t popId, size_t gen) {
      sOut << gen << "," << popId << "," << stats << std::endl;
    };

    ctx.individualReportCallback = [&iOut](std::string s, uint32_t popId,
                                           size_t gen) {
      iOut << gen << ',' << popId << "," << s << std::endl;
    };

    ctxGen(ctx, numCompetitions);

    return ctx;
  }
};

struct RunConfig {
  std::string rootResultsLoc;
  std::vector<ExperimentConfig> experiments;
};

void evolve(size_t numGens, Context ctx);

std::vector<folly::SemiFuture<int>> runExperiment(ExperimentConfig exp,
                                                  fs::path resPath);

void runFromConfig(RunConfig cfg, std::string configFile);

RunConfig parseTomlConfig(fs::path configFile);

#endif //MPEV_EXPERIMENTRUNNER_H
