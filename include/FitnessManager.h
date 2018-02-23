//
// Created by Oliver Downard on 21/01/2018.
//

#pragma once

#include "Individual.h"
#include <optional>
#include <unordered_map>

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
  virtual void
      setCompMap(std::unordered_map<uint32_t, std::vector<uint32_t>>) = 0;
};
