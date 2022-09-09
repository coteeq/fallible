cmake_minimum_required(VERSION 3.14)

project(wheels-result)

option(WHEELS_RESULT_TESTS "Enable wheels-result tests" OFF)
option(WHEELS_RESULT_EXAMPLES "Enable wheels-result examples" OFF)
option(WHEELS_RESULT_DEVELOPER "Wheels-result developer mode" OFF)

include(cmake/CompileOptions.cmake)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin")

add_subdirectory(third-party)
add_subdirectory(wheels)

if(WHEELS_RESULT_TESTS OR WHEELS_RESULT_DEVELOPER)
    add_subdirectory(tests)
endif()

if(WHEELS_RESULT_EXAMPLES OR WHEELS_RESULT_DEVELOPER)
    add_subdirectory(examples)
endif()
