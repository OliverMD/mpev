//
// Created by Oliver Downard on 19/12/2017.
//
#include "include/Fitness.h"
#include "include/Population.h"
#include <iostream>
#include <random>

float identityFitnessFunc(const IndividualRep *, const IndividualRep *) {
  return 0;
}

uint32_t FitnessManager::readySignal(Population *const pop) {
  if (pops.count(pop->getId()) < 1) {
    pops.insert({pop->getId(), pop});
  }

  // runEvaluation may alter the sequence number, so take a copy here.
  uint32_t oldSeq = seqNo;
  if (pops.size() == numPops) {
    // Ready to run the evaluation
    runEvaluation();
  }
  return oldSeq;
}

std::optional<uint32_t> FitnessManager::lastEvaluation() {
  if (seqNo == 0) {
    return {};
  } else {
    return seqNo - 1;
  }
}

void FitnessManager::runEvaluation() {
  // TODO: Have changeable policies

  // TODO: Use structured bindings
  // TODO: Parallelise this
  // There is a potential tradeoff to be had here between parralleising it so
  // each population is evaluated by a different thread or updating the fitness
  // of all those individuals that take part in the tournament.
  for (auto &kv : pops) {
    // Need to assign a fitness to each Individual in each population
    // O(nm)

    FitnessManager::iterator oppStart = begin(kv.first);
    FitnessManager::iterator oppEnd = end(kv.first);

    // TODO: Not a fan of storing all these, seems like a waste of memory
    std::vector<IndividualRep *> rPops;
    rPops.reserve(kv.second->size());
    std::sample(oppStart, oppEnd, std::back_inserter(rPops), kv.second->size(),
                std::mt19937{std::random_device{}()});

    auto riter = std::begin(rPops);
    for (auto &ind : *kv.second) {
      ind.fitness = fitnessFunction(ind.representation.get(), *riter++);
    }
  }

  seqNo++;
  pops.clear();
}

FitnessManager::iterator::iterator() {}

FitnessManager::iterator::iterator(std::initializer_list<uint32_t> l)
    : excludedPops{l} {}

FitnessManager::iterator::iterator(const iterator &other)
    : excludedPops{other.excludedPops}, popIt{other.popIt}, indIt{other.indIt},
      endIt{other.endIt} {}

FitnessManager::iterator::iterator(std::initializer_list<uint32_t> l,
                                   PopStore::const_iterator psi,
                                   PopStore::const_iterator epsi)
    : excludedPops{l}, popIt{psi}, endPopIt{epsi} {
  if (popIt != endPopIt) {
    indIt = std::begin(*popIt->second);
    endIt = std::end(*popIt->second);
  }
}

FitnessManager::iterator::~iterator() {}

FitnessManager::iterator &FitnessManager::iterator::operator++() {
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

FitnessManager::iterator FitnessManager::iterator::operator++(int num) {
  auto ret = *this;
  this->operator++();
  return ret;
}

bool FitnessManager::iterator::operator!=(const iterator &other) const {
  return !(*this == other);
}

bool FitnessManager::iterator::operator==(const iterator &other) const {
  return (indIt == other.indIt) && (popIt == other.popIt) &&
         (endPopIt == other.endPopIt) && (excludedPops == other.excludedPops) &&
         (endIt == other.endIt);
}

FitnessManager::iterator &FitnessManager::iterator::
operator=(const iterator &other) {
  excludedPops = other.excludedPops;
  popIt = other.popIt;
  endPopIt = other.endPopIt;
  indIt = other.indIt;
  endIt = other.endIt;
  return *this;
}

FitnessManager::iterator::pointer FitnessManager::iterator::operator*() const {
  return indIt->representation.get();
}

FitnessManager::iterator::reference FitnessManager::iterator::
operator->() const {
  return *indIt->representation.get();
}

void swap(FitnessManager::iterator &lhs, FitnessManager::iterator &rhs) {
  FitnessManager::iterator tmp = rhs;
  rhs = lhs;
  lhs = tmp;
}