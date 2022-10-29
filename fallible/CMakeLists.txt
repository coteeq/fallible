add_library(fallible
		error/codes.hpp
		error/codes.cpp
		error/error.hpp
		error/error.cpp
		error/throw.hpp
		result/result.hpp
		result/make.hpp
		result/make.cpp
		rt/panic.hpp
		rt/panic.cpp
		rt/panicker.hpp
		rt/panicker.cpp
		rt/abort.hpp
		rt/abort.cpp
)

# Dependencies

target_link_libraries(fallible
		wheels
		nlohmann_json::nlohmann_json)

target_include_directories(fallible PUBLIC ..)
