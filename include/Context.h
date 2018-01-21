//
// Created by Oliver Downard on 02/01/2018.
//
#pragma once

#include "Individual.h"
#include "FitnessManager.h"

#include <functional>

/**
 * Context class that stores various helper functions and utilities and
 * parameters used during evolution.
 */
struct Context {
  using IndividualMaker = std::function<decltype(makeRandomIntIndivdual)>;
  using CrossoverFunc = std::function<decltype(crossoverIntIndividuals)>;
  using MutationFunc = std::function<decltype(mutateIntIndividual)>;
  using PopSizeType = uint32_t;

  Context() {}
  Context(IndividualMaker maker, CrossoverFunc cross, MutationFunc mut,
          size_t tSize)
      : individualMaker{maker}, crossoverFunc{cross},
        mutationFunc{mut}, tournSize{tSize} {}

  std::unique_ptr<FitnessManager> fitnessManager;
  IndividualMaker individualMaker;

  /**
   * The crossover function is responsible for crossover probability
   */
  CrossoverFunc crossoverFunc;

  /**
   * The mutation function is responsible for the mutation probability
   */
  MutationFunc mutationFunc;
  PopSizeType popSize;
  size_t tournSize;
};

Context makeDefaultContext();
