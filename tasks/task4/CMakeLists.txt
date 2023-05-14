add_executable(task4 task4.cpp)
target_link_libraries(task4 GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(task4)
