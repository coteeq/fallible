include(FetchContent)

# --------------------------------------------------------------------

# Offline mode (uncomment next line to enable)
# set(FETCHCONTENT_FULLY_DISCONNECTED ON)

# set(FETCHCONTENT_QUIET OFF)

# --------------------------------------------------------------------

# Core components

FetchContent_Declare(
        wheels
        GIT_REPOSITORY https://gitlab.com/Lipovsky/wheels.git
        GIT_TAG master
)
FetchContent_MakeAvailable(wheels)

# --------------------------------------------------------------------

message(STATUS "FetchContent: fmtlib")

FetchContent_Declare(
        fmtlib
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG 8.0.1
)
FetchContent_MakeAvailable(fmtlib)
