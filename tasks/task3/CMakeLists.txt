add_executable(task3 task3.cpp)
target_link_libraries(task3 GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(task3)
