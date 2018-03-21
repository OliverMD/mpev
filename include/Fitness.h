//
// Created by Oliver Downard on 19/12/2017.
//
#pragma once

#include <initializer_list>
#include <optional>
#include <random>
#include <unordered_map>

#include "FitnessManager.h"
#include "Individual.h"
#include "Population.h"

float identityFitnessFunc(const IndividualRep *, const IndividualRep *);

/**
 * Just sum together scores
 */
template <decltype(identityFitnessFunc) FitFunc> class DefaultFitnessEv {
public:
  void processPair(const IndividualRep *a, const IndividualRep *b) {
    fitnessMap[a] += FitFunc(a, b);
  }

  float getFitness(const IndividualRep *a) { return fitnessMap[a]; }

  void clear() { fitnessMap.clear(); }

private:
  std::unordered_map<const IndividualRep *, float> fitnessMap;
};

/**
 * TODO: Extremely not thread safe at, will have data races
 */
template <typename FitEv> class CoevFitnessManager : public FitnessManager {
public:
  // using FitnessFunction = std::function<decltype(identityFitnessFunc)>;
  using PopStore = std::unordered_map<uint32_t, std::vector<Individual> *const>;

  /**
   *
   * @param numOfPops: The number of populations to wait for and evaluate
   * fitness for.
   */
  CoevFitnessManager(Context &ctx, uint16_t numOfPops, size_t numOpps,
                     FitEv e = FitEv{})
      : ctx{ctx}, numPops{numOfPops}, numOfOpponents{numOpps}, seqNo{0}, ev{e} {
  }

  /**
   * Signal that the population pop is ready for evaluation.
   * NOTE: This will run the actual evaluation if this is the last population
   * that is expected.
   * @param pop
   * @return the evaluation sequence number that the population will be next
   * used for
   */
  uint32_t readySignal(Population *const pop) override {
    return readySignal(&pop->currentInds, pop->getId());
  }

  uint32_t readySignal(std::vector<Individual> * const pop, uint32_t popId) override {
    if (pops.count(popId) < 1) {
      pops.insert({popId, pop});
    }

    // runEvaluation may alter the sequence number, so take a copy here.
    uint32_t oldSeq = seqNo;
    if (pops.size() == numPops) {
      // Ready to run the evaluation
      runEvaluation();
    }
    return oldSeq;
  }

  /**
   *
   * @return the sequence number of the last evaluation
   */
  std::optional<uint32_t> lastEvaluation() override {
    if (seqNo == 0) {
      return {};
    } else {
      return seqNo - 1;
    }
  }

  void setCompMap(
      std::unordered_map<uint32_t, std::vector<uint32_t>> newMap) override {
    compMap = newMap;
  }

private:
  void runEvaluation() {
    // TODO: Parallelise this
    // There is a potential tradeoff to be had here between parralleising it so
    // each population is evaluated by a different thread or updating the
    // fitness of all those individuals that take part in the tournament.

    std::vector<uint32_t> allIds;

    for (const auto &kv : pops) {
      allIds.push_back(kv.first);
    }

    for (auto &kv : pops) {
      // Need to assign a fitness to each Individual in each population
      // O(nm)

      std::vector<uint32_t> oppPops;
      if (compMap.find(kv.first) != std::end(compMap)) {
        oppPops = compMap.at(kv.first);
      } else {
        oppPops = allIds;
        oppPops.erase(
            std::find(std::begin(oppPops), std::end(oppPops), kv.first));
      }
      if (numPops == 1) {
        oppPops = allIds;
      }

      for (auto &ind : *kv.second) {
        std::vector<uint32_t> rPops;

        while (rPops.size() < numOfOpponents) {
          std::sample(std::begin(oppPops), std::end(oppPops),
                      std::back_inserter(rPops), numOfOpponents - rPops.size(),
                      ctx.rng);
        }

        for (const auto p : rPops) {
          // Randomly choose a valid population
          // Then choose a random individual from the population.
          std::uniform_int_distribution<size_t> dis{0, pops.at(p)->size() - 1};
          ev.processPair(ind.representation.get(),
                         pops.at(p)->at(dis(ctx.rng)).representation.get());
        }
      }

      for (auto &ind : *kv.second) {
        ind.fitness = ev.getFitness(ind.representation.get());
      }
    }

    seqNo++;
    ev.clear();
    pops.clear();
  }
  const uint16_t numPops;
  PopStore pops;
  uint32_t seqNo;
  size_t numOfOpponents;
  FitEv ev;
  Context &ctx;
  std::unordered_map<uint32_t, std::vector<uint32_t>> compMap;
};
