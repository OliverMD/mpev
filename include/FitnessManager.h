//
// Created by Oliver Downard on 21/01/2018.
//

#pragma once

#include <optional>
#include "Individual.h"

class Population;

/**
 * Interface for a FitnessManager
 */
class FitnessManager {
public:
  virtual uint32_t readySignal(Population *const pop) = 0;
  virtual uint32_t readySignal(std::vector<Individual> *const pop,
                               uint32_t popId) = 0;
  virtual std::optional<uint32_t> lastEvaluation() = 0;
};
