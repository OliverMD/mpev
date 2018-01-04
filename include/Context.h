//
// Created by Oliver Downard on 02/01/2018.
//
#pragma once

#include "Individual.h"

class FitnessManager;
/**
 * Context class that stores various helper functions and utilities and
 * parameters used during evolution.
 */
struct Context {
  using IndividualMaker = decltype(&makeRandomIntIndivdual);
  using PopSizeType = uint32_t;

  std::unique_ptr<FitnessManager> fitnessManager;
  IndividualMaker individualMaker;
  PopSizeType popSize;
};
