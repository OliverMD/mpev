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
  CoevFitnessManager(uint16_t numOfPops, size_t numOpps, FitEv e = FitEv{})
      : numPops{numOfPops}, numOfOpponents{numOpps}, seqNo{0}, ev{e} {}

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

  /**
   * Provides a mechanism to iterate over IndividualRepresentations
   */
  class iterator {
  public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = IndividualRep *;
    using reference = IndividualRep &;
    using pointer = value_type;
    using difference_type = size_t;

    iterator() {}
    iterator(std::initializer_list<uint32_t> l) : excludedPops{l} {}
    iterator(const iterator &other)
        : excludedPops{other.excludedPops}, popIt{other.popIt},
          indIt{other.indIt}, endIt{other.endIt} {}
    iterator(std::initializer_list<uint32_t> l, PopStore::const_iterator psi,
             PopStore::const_iterator epsi)
        : excludedPops{l}, popIt{psi}, endPopIt{epsi} {
      if (popIt != endPopIt) {
        indIt = std::begin(*popIt->second);
        endIt = std::end(*popIt->second);
      }
    }
    ~iterator() {}

    iterator &operator=(const iterator &other) {
      excludedPops = other.excludedPops;
      popIt = other.popIt;
      endPopIt = other.endPopIt;
      indIt = other.indIt;
      endIt = other.endIt;
      return *this;
    }
    bool operator==(const iterator &other) const {
      return (indIt == other.indIt) && (popIt == other.popIt) &&
             (endPopIt == other.endPopIt) &&
             (excludedPops == other.excludedPops) && (endIt == other.endIt);
    }
    bool operator!=(const iterator &other) const { return !(*this == other); }
    iterator &operator++() {
      // Urgh, this is horrible
      if (popIt != endPopIt) {
        if (indIt != endIt) {
          ++indIt;
        }
        if (indIt == endIt) {
          ++popIt;
          if (popIt != endPopIt) {
            while (std::find(std::begin(excludedPops), std::end(excludedPops),
                             popIt->first) != std::end(excludedPops)) {
              ++popIt;
              if (popIt == endPopIt) {
                break;
              }
            }
          }

          if (popIt != endPopIt) {
            indIt = std::begin(*popIt->second);
            endIt = std::end(*popIt->second);
          } else {
            indIt = {};
            endIt = {};
          }
        }
      }
      return *this;
    }
    iterator operator++(int) {
      auto ret = *this;
      this->operator++();
      return ret;
    }

    pointer operator*() const { return indIt->representation.get(); }
    reference operator->() const { return *indIt->representation.get(); }

    friend void swap(iterator &lhs, iterator &rhs);

  private:
    std::vector<uint32_t> excludedPops;
    PopStore::const_iterator popIt;
    PopStore::const_iterator endPopIt;
    std::vector<Individual>::const_iterator indIt;
    std::vector<Individual>::const_iterator endIt;
  };

  iterator begin(uint32_t exclude) {
    return {{exclude}, std::begin(pops), std::end(pops)};
  }

  iterator end(uint32_t exclude) {
    return {{exclude}, std::end(pops), std::end(pops)};
  }

private:
  void runEvaluation() {
    // TODO: Have changeable policies

    // TODO: Use structured bindings
    // TODO: Parallelise this
    // There is a potential tradeoff to be had here between parralleising it so
    // each population is evaluated by a different thread or updating the
    // fitness of all those individuals that take part in the tournament.
    for (auto &kv : pops) {
      // Need to assign a fitness to each Individual in each population
      // O(nm)

      CoevFitnessManager::iterator oppStart = begin(kv.first);
      CoevFitnessManager::iterator oppEnd = end(kv.first);
      if (numPops == 1) {
        oppStart = {{}, std::begin(pops), std::end(pops)};
        oppEnd = {{}, std::end(pops), std::end(pops)};
      }

      for (auto &ind : *kv.second) {
        std::vector<IndividualRep *> rPops;
        rPops.reserve(kv.second->size());
        std::sample(oppStart, oppEnd, std::back_inserter(rPops), numOfOpponents,
                    std::mt19937{std::random_device{}()});

        auto riter = std::begin(rPops);
        for (const auto &iter : rPops) {
          ev.processPair(ind.representation.get(), iter);
        }
      }

      for (auto &ind : *kv.second) {
        ind.fitness = ev.getFitness(ind.representation.get());
      }
    }

    seqNo++;
    pops.clear();
  }
  const uint16_t numPops;
  PopStore pops;
  uint32_t seqNo;
  size_t numOfOpponents;
  FitEv ev;
};

template <typename FitEv>
void swap(typename CoevFitnessManager<FitEv>::iterator &lhs,
          typename CoevFitnessManager<FitEv>::iterator &rhs) {
  typename CoevFitnessManager<FitEv>::iterator tmp = rhs;
  rhs = lhs;
  lhs = tmp;
}