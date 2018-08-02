set(EXPERIMENT_SOURCE main.cpp)

include_directories(AFTER ${FOLLY_INCLUDES})

add_library(mpevrunner ExperimentRunner.cpp)
add_executable(mpev_experiments ${EXPERIMENT_SOURCE})
target_link_libraries(mpev_experiments ${FOLLY} Evo mpevrunner glog::glog)

install(FILES ExperimentRunner.h DESTINATION include)
install(TARGETS mpevrunner DESTINATION lib)
install(TARGETS mpev_experiments DESTINATION bin)