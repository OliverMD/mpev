//
// Created by Oliver Downard on 17/12/2017.
//
#include "include/Population.h"

std::atomic<uint32_t> Population::CURRENTID = 0;

std::unique_ptr<PopulationState> InitialPopState::execute(Population &pop) {
  return nullptr;
}
