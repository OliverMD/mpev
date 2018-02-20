//
// Created by Oliver Downard on 02/01/2018.
//
#pragma once

#include "Individual.h"
#include "FitnessManager.h"
#include "Stats.h"

#include <functional>

/**
 * Context class that stores various helper functions and utilities and
 * parameters used during evolution.
 */
struct Context {
  using IndividualMaker = std::function<decltype(makeRandomIntIndivdual)>;
  using CrossoverFunc = std::function<decltype(crossoverIntIndividuals)>;
  using MutationFunc = std::function<decltype(mutateIntIndividual)>;
  using ReporterCallback = std::function<void(PopulationStats, uint32_t, size_t)>;
  using ObjectiveFunc = std::function<float(const IndividualRep*)>;

  using VariationSelector =
      std::function<Individual &(std::vector<Individual> &)>;

  using NewPop = std::vector<Individual>;
  using OldPop = std::vector<Individual>;

  using SurvivalSelector = std::function<Individual &(NewPop &, OldPop &)>;

  using VarySelectorCreator = std::function<VariationSelector(Context&)>;
  using SurvivalSelectorCreator = std::function<SurvivalSelector(Context&)>;

  using PopSizeType = uint32_t;

  Context() {}
  Context(IndividualMaker maker, CrossoverFunc cross, MutationFunc mut,
          size_t tSize, size_t popCount)
      : individualMaker{maker}, crossoverFunc{cross},
        mutationFunc{mut}, tournSize{tSize}, objectiveReportCallback{nullptr},
        subjectiveReportCallback{nullptr}, populationCount{popCount} {}

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
  ReporterCallback objectiveReportCallback;
  ReporterCallback subjectiveReportCallback;
  ObjectiveFunc objectiveFunc;

  VarySelectorCreator varySelectorCreator;
  SurvivalSelectorCreator survivalSelectorCreator;

  size_t populationCount;
};

Context makeDefaultContext();
