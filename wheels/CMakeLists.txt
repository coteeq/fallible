add_library(wheels-result
	result/error.hpp
	result/result.hpp
	result/make.hpp
	result/make.cpp
)

target_link_libraries(wheels-result wheels)
target_include_directories(wheels-result PUBLIC ..)
