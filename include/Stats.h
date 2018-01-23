//
// Created by Oliver Downard on 23/01/2018.
//
#pragma once

#include <ostream>

struct PopulationStats {
  float maxFitness;
  float minFitness;

  float meanFitness;

  float medianFitness;
  float upperQuartileFitness;
  float lowerQuartileFitness;
};

std::ostream &operator<<(std::ostream &os, const PopulationStats &stats);
