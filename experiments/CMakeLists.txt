set(EXPERIMENT_SOURCE main.cpp)

    add_executable(Experiments ${EXPERIMENT_SOURCE})
        target_link_libraries(Experiments Evo)