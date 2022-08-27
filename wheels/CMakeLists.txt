add_library(wheels-result
		result/error_codes.hpp
		result/error.hpp
		result/error.cpp
		result/result.hpp
		result/make.hpp
		result/make.cpp
)

# Dependencies

target_link_libraries(wheels-result
		wheels
		nlohmann_json::nlohmann_json)

target_include_directories(wheels-result PUBLIC ..)
