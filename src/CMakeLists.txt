set(SOURCE_FILES GreaterThanGame.cpp Population.cpp PopulationStates.cpp Fitness.cpp ${INCLUDES})
set(EXPERIMENT_SOURCE main.cpp)

add_library(Evo ${SOURCE_FILES})
add_executable(Experiment ${EXPERIMENT_SOURCE})