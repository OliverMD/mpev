//
// Created by Oliver Downard on 19/12/2017.
//

#include "include/PopulationStates.h"

std::unique_ptr<PopulationState> EvaluateFitnessState::execute(Population &pop) {
  // Slightly complex under coevolution as we need other populations to

  // Tell the manager this population wants to be processed, it may run straight
  // away if all other relevant populations are ready.
  // Every time this is run check if the fitness evaluation has been done and
  // change states if it has.
}
