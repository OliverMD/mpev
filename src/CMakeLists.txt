set(SOURCE_FILES GreaterThanGame.cpp)
set(EXPERIMENT_SOURCE main.cpp)

add_library(GreaterThanGame ${SOURCE_FILES})
add_executable(Experiment ${EXPERIMENT_SOURCE})