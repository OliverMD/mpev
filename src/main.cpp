#include <array>
#include <include/Context.h>
#include <include/Fitness.h>
#include <include/Individual.h>
#include <include/PopulationStates.h>
#include <iostream>
#include <random>

namespace ExpOne {

constexpr size_t NumBits = 100;
constexpr float CrossoverProb = 0.1;
constexpr float MutationProb = 0.01;

template <size_t size> class OnesIndRep : public IndividualRep {
public:
  OnesIndRep() : bits{} { bits.resize(size); }
  std::string name() { return "Ones"; }
  size_t getNumOnes() const {
    uint cnt = 0;
    for (const auto b : bits) {
      if (b) {
        cnt += 1;
      }
    }
    return cnt;
  }

  OnesIndRep(std::vector<bool> a) : bits{a} { assert(a.size() == size); }

  OnesIndRep(OnesIndRep &other) = default;

  constexpr size_t getSize() const { return size; }

  bool flipBit(size_t i) {
    bits[i] = ~bits[i];
    return bits[i];
  }

  /**
   * [0, 1, 2, 3, 4]
   *
   * Slice(1, 4) returns [1, 2, 3]
   *
   * @param front The start of the slice, inclusive
   * @param back One past the end of the slice
   * @return
   */
  std::vector<bool> getSlice(size_t front, size_t back) {
    assert(back <= bits.size());
    assert(front <= back);
    std::vector<bool> ret;
    ret.reserve(back - front);
    for (size_t i = front; i < back; ++i) {
      ret.emplace_back(bits[i]);
    }
    return ret;
  }

private:
  std::vector<bool> bits;
};

using Rep = OnesIndRep<NumBits>;

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
    auto x = aa->getSlice(0, cPoint);
    auto y = bb->getSlice(0, cPoint);

    x.insert(std::end(x), std::begin(bb->getSlice(cPoint, bb->getSize())),
             std::end(bb->getSlice(cPoint, bb->getSize())));

    y.insert(std::end(y), std::begin(aa->getSlice(cPoint, aa->getSize())),
             std::end(aa->getSlice(cPoint, aa->getSize())));

    ret.emplace_back(std::make_unique<Rep>(x), 0);
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

  return aa->getNumOnes() > bb->getNumOnes() ? 1.0 : 0.0;
}

Individual mutateOnesInd(Individual &a) {
  static std::random_device
      rd; // Will be used to obtain a seed for the random number engine
  static std::mt19937 gen(
      rd()); // Standard mersenne_twister_engine seeded with rd()
  static std::uniform_real_distribution<float> dis{};
  Rep *aa = dynamic_cast<Rep *>(a.representation.get());
  std::vector<bool> r = aa->getSlice(0, NumBits);
  for (size_t i = 0; i < r.size(); ++i) {
    float z = dis(gen);
    if (z < MutationProb) {
      r[i] = !r[i];
    }
  }

  return {std::make_unique<Rep>(r), 0};
}
} // namespace ExpOne

// Experiment 1
int main() {
  // 1. Generate populations
  // 2. For n generations
  //    i. calculate fitness of each individual in each population
  //        a. for each individual pick x random individuals from all other
  //        populations b. test against all competitors and assign fitness
  //    ii. selection for variation
  //    iii. generate new individuals
  //    iv. selection for replacement/survival

  Context ctx = makeDefaultContext();
  ctx.tournSize = 5;
  ctx.mutationFunc = ExpOne::mutateOnesInd;
  ctx.crossoverFunc = ExpOne::crossOnesInds;
  ctx.individualMaker = ExpOne::makeOnesInd;

  ctx.popSize = 100;

  ctx.fitnessManager = std::make_unique<
      CoevFitnessManager<DefaultFitnessEv<ExpOne::fitnessFunc>>>(2, 10);

  std::array<Population, 2> pops{
      Population{std::make_unique<InitialPopState>(ctx)},
      Population{std::make_unique<InitialPopState>(ctx)}};

  std::array<size_t, 2> gens{0, 0};

  constexpr size_t numGens = 1000;

  while (gens[0] < numGens || gens[1] < numGens) {
    if (gens[0] < numGens) {
      pops[0].step();
      if (pops[0].getState()->name() == VariationState::Name) {
        std::cout << "0: gen: " << gens[0] << " - " << pops[0].getStats()
                  << std::endl;
        gens[0] += 1;
      }
    }
    if (gens[1] < numGens) {
      pops[1].step();
      if (pops[1].getState()->name() == VariationState::Name) {
        std::cout << "1: gen: " << gens[1] << " - " << pops[1].getStats()
                  << std::endl;
        gens[1] += 1;
      }
    }
  }
  return 0;
}