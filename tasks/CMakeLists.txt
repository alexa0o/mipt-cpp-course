project(MIPT_CPP)
CMAKE_MINIMUM_REQUIRED(VERSION 3.22)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Fetch googletest
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG main
)

FetchContent_MakeAvailable(googletest)

set(TASK "" CACHE STRING "Task id")

if (TASK GREATER 0)
    set(TASK_NAME "task${TASK}")
else()
    message(FATAL_ERROR "Wrong task id format. Use -DTASK option to specify task id.")
endif()

message(STATUS "Building tests for ${TASK_NAME}")

enable_testing()
add_compile_options(-Wall -Wextra)
include_directories("${TASK_NAME}")
add_subdirectory("${TASK_NAME}")
