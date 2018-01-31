set(SOURCE_FILES GreaterThanGame.cpp Population.cpp PopulationStates.cpp Fitness.cpp ${INCLUDES} Individual.cpp Context.cpp Stats.cpp Selectors.cpp)

add_library(Evo ${SOURCE_FILES})
