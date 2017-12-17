add_executable(BasicTests basicTests.cpp)
target_link_libraries(BasicTests gtest_main Evo)
add_test(NAME basic_tests COMMAND BasicTests)