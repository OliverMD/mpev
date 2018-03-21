//
// Created by Oliver Downard on 21/01/2018.
//

#include "include/Context.h"

Context makeDefaultContext(unsigned int seed) {
  return Context{makeRandomIntIndivdual,
                 crossoverIntIndividuals,
                 mutateIntIndividual,
                 1,
                 1,
                 seed};
}