//
// Created by Oliver Downard on 02/01/2018.
//
#pragma once

#include "Individual.h"
#include "Fitness.h"

/**
 * Context class that stores various helper functions and utilities and
 * parameters used during evolution.
 */
struct Context {
  using IndividualMaker = decltype(makeRandomIntIndivdual);

  std::unique_ptr<FitnessManager> fitnessManager;
};
