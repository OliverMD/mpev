//
// Created by Oliver Downard on 21/01/2018.
//

#pragma once

#include <optional>

class Population;

/**
 * Interface for a FitnessManager
 */
class FitnessManager {
public:
  virtual uint32_t readySignal(Population *const pop) = 0;
  virtual std::optional<uint32_t> lastEvaluation() = 0;
};
