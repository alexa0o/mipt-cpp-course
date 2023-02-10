add_executable(task2 task2.cpp)
target_link_libraries(task2 GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(task2)
