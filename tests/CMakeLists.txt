add_executable(fallible-tests
	all.cpp
	context.cpp
	error.cpp
	result.cpp)

target_link_libraries(fallible-tests fallible wheels)
