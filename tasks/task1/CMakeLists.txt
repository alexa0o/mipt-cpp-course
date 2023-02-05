add_executable(task1 task1.cpp)
target_link_libraries(task1 GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(task1)
