//
// Created by Oliver Downard on 23/01/2018.
//
#include "include/Stats.h"

std::ostream &operator<<(std::ostream &os, const PopulationStats &stats) {
  os << stats.maxFitness << "," << stats.minFitness << "," << stats.meanFitness
     << "," << stats.medianFitness << "," << stats.upperQuartileFitness << ","
     << stats.lowerQuartileFitness;
  return os;
}