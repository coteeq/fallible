add_executable(wheels-result-tests
	all.cpp
	error.cpp
	result.cpp)

target_link_libraries(wheels-result-tests wheels-result wheels)
