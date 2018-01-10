//
// Created by Oliver Downard on 19/12/2017.
//
#pragma once

#include <initializer_list>
#include <optional>
#include <unordered_map>

#include "Individual.h"
#include "Population.h"

float identityFitnessFunc(const IndividualRep *, const IndividualRep *);

/**
 * TODO: Extremely not thread safe at, will have data races
 */
class FitnessManager {
public:
  using FitnessFunction = std::function<decltype(identityFitnessFunc)>;
  using PopStore = std::unordered_map<uint32_t, Population *const>;

  /**
   *
   * @param numOfPops: The number of populations to wait for and evaluate
   * fitness for.
   */
  FitnessManager(uint16_t numOfPops)
      : numPops{numOfPops}, fitnessFunction{&identityFitnessFunc}, seqNo{0} {
  }

  /**
   * Signal that the population pop is ready for evaluation.
   * NOTE: This will run the actual evaluation if this is the last population
   * that is expected.
   * @param pop
   * @return the evaluation sequence number that the population will be next
   * used for
   */
  uint32_t readySignal(Population *const pop);

  /**
   *
   * @return the sequence number of the last evaluation
   */
  std::optional<uint32_t> lastEvaluation();

  void changeFitnessFunction(FitnessFunction func) { fitnessFunction = func; }

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

    iterator();
    iterator(std::initializer_list<uint32_t> l);
    iterator(const iterator &);
    iterator(std::initializer_list<uint32_t> l, PopStore::const_iterator psi,
             PopStore::const_iterator epsi);
    ~iterator();

    iterator &operator=(const iterator &);
    bool operator==(const iterator &) const;
    bool operator!=(const iterator &) const;
    iterator &operator++();
    iterator operator++(int);

    pointer operator*() const;
    reference operator->() const;

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
  void runEvaluation();
  const uint16_t numPops;
  PopStore pops;
  FitnessFunction fitnessFunction;
  uint32_t seqNo;
};

void swap(FitnessManager::iterator &lhs, FitnessManager::iterator &rhs);