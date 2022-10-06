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

# Json

# https://github.com/nlohmann/json.git
# https://json.nlohmann.me/integration/cmake/#fetchcontent

set(JSON_Install ON)

FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/ArthurSonzogni/nlohmann_json_cmake_fetchcontent
        GIT_PROGRESS TRUE
        GIT_SHALLOW TRUE
        GIT_TAG v3.11.2
)
FetchContent_MakeAvailable(nlohmann_json)
