set(EXPERIMENT_SOURCE main.cpp)

add_library(mpevrunner ExperimentRunner.cpp)
add_executable(mpev_experiments ${EXPERIMENT_SOURCE})
target_link_libraries(mpev_experiments Evo mpevrunner)

install(FILES ExperimentRunner.h DESTINATION include)
install(TARGETS mpevrunner DESTINATION lib)
install(TARGETS mpev_experiments DESTINATION bin)