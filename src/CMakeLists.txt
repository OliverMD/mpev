set(SOURCE_FILES GreaterThanGame.cpp Population.cpp PopulationStates.cpp Fitness.cpp ${INCLUDES} Individual.cpp Context.cpp)
set(EXPERIMENT_SOURCE main.cpp)

add_library(Evo ${SOURCE_FILES})
add_executable(Experiment ${EXPERIMENT_SOURCE})