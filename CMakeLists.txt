cmake_minimum_required(VERSION 3.14)

project(fallible)

option(FALLIBLE_TESTS "Enable Fallible tests" OFF)
option(FALLIBLE_EXAMPLES "Enable Fallible examples" OFF)
option(FALLIBLE_DEVELOPER "Fallible developer mode" OFF)

include(cmake/CompileOptions.cmake)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin")

add_subdirectory(third-party)
add_subdirectory(fallible)

if(FALLIBLE_TESTS OR FALLIBLE_DEVELOPER)
    add_subdirectory(tests)
endif()

if(FALLIBLE_EXAMPLES OR FALLIBLE_DEVELOPER)
    add_subdirectory(examples)
endif()
