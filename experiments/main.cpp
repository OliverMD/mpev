#include <array>
#include <include/Context.h>
#include <include/Fitness.h>
#include <include/PopulationStates.h>
#include <iostream>
#include <fstream>

#include "include/Selectors.h"

#include "OnesInd.h"

namespace ExpOne {

constexpr size_t NumBits = 100;
constexpr float CrossoverProb = 0;
constexpr float MutationProb = 0.05;

using Rep = OnesIndRep<NumBits, 1>;

Individual makeOnesInd() { return {std::make_unique<Rep>(), 0}; }

std::vector<Individual> crossOnesInds(Individual &a, Individual &b) {
  static std::random_device
      rd; // Will be used to obtain a seed for the random number engine
  static std::mt19937 gen(
      rd()); // Standard mersenne_twister_engine seeded with rd()
  static std::uniform_real_distribution<float> dis(rd());
  static std::uniform_int_distribution<size_t> pDis(0, NumBits);

  std::vector<Individual> ret{};
  Rep *aa = dynamic_cast<Rep *>(a.representation.get());
  Rep *bb = dynamic_cast<Rep *>(b.representation.get());

  if (dis(gen) < CrossoverProb) {
    // Do the crossover
    size_t cPoint = pDis(gen);
    auto x = aa->getSlice(0, 0, cPoint);
    auto y = bb->getSlice(0, 0, cPoint);

    x.insert(std::end(x), std::begin(bb->getSlice(0, cPoint, bb->getSize())),
             std::end(bb->getSlice(0, cPoint, bb->getSize())));

    y.insert(std::end(y), std::begin(aa->getSlice(0, cPoint, aa->getSize())),
             std::end(aa->getSlice(0, cPoint, aa->getSize())));

    ret.emplace_back(std::make_unique<Rep>(std::array<std::vector<bool>, 1>{x}),
                     0);
    // ret.emplace_back(std::make_unique<Rep>(y), 0);
  } else {
    // Return the individuals unblemished
    ret.emplace_back(std::make_unique<Rep>(*aa), 0);
    // ret.emplace_back(std::make_unique<Rep>(*bb), 0);
  }
  return ret;
}

float fitnessFunc(const IndividualRep *a, const IndividualRep *b) {
  const Rep *aa = dynamic_cast<const Rep *>(a);
  const Rep *bb = dynamic_cast<const Rep *>(b);

  return aa->getNumOnes(0) > bb->getNumOnes(0) ? 1.0 : 0.0;
}

Individual mutateOnesInd(Individual &a) {
  static std::random_device
      rd; // Will be used to obtain a seed for the random number engine
  static std::mt19937 gen(
      rd()); // Standard mersenne_twister_engine seeded with rd()
  static std::uniform_real_distribution<float> dis{};
  static std::uniform_int_distribution<bool> valDis{};
  Rep *aa = dynamic_cast<Rep *>(a.representation.get());
  std::vector<bool> r = aa->getSlice(0, 0, NumBits);
  for (size_t i = 0; i < r.size(); ++i) {
    float z = dis(gen);
    if (z < MutationProb) {
      r[i] = valDis(gen);
    }
  }

  return {std::make_unique<Rep>(std::array<std::vector<bool>, 1>{r}), 0};
}
} // namespace ExpOne

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

void experimentOne(std::ofstream& out) {
  Context ctx = makeDefaultContext();
  ctx.tournSize = 5;
  ctx.mutationFunc = ExpOne::mutateOnesInd;
  ctx.crossoverFunc = ExpOne::crossOnesInds;
  ctx.individualMaker = ExpOne::makeOnesInd;

  ctx.varySelectorCreator = createRouletteSelect;
  ctx.survivalSelectorCreator = createTopSelector;

  ctx.objectiveFunc = [](const IndividualRep* rep) {
    const ExpOne::Rep* a = dynamic_cast<const ExpOne::Rep *>(rep);
    return a->getNumOnes(0);
  };

  ctx.popSize = 25;

  ctx.fitnessManager = std::make_unique<
      CoevFitnessManager<DefaultFitnessEv<ExpOne::fitnessFunc>>>(2, 15);

  ctx.reporterCallback = [&out](PopulationStats stats, uint32_t popId, size_t gen) {
    out << gen << "," << popId << "," << stats << std::endl;
  };

  evolve(600, std::move(ctx), 2);
}

int main(int argc, char* argv[]) {

  std::string resFile = "results.csv";

  if (argc == 2) {
    resFile = argv[1];
  }

  std::ofstream oFile(resFile, std::ios::out);
  oFile << "gen,pop,max,min,mean,median,upper,lower" << std::endl;

  experimentOne(oFile);

  oFile.close();
  return 0;
}