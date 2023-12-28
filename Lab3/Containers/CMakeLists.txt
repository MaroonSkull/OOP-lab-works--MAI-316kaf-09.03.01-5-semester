cmake_minimum_required (VERSION 3.8)
project (${PROJECT_NAME})


add_library (Containers INTERFACE
	List.hpp
	LinkedList.hpp
	ArrayList.hpp
)
add_dependencies (${PROJECT_NAME} Containers)

target_include_directories (Containers INTERFACE ./)

target_compile_features (Containers INTERFACE cxx_std_20)

target_link_libraries (Containers
	INTERFACE Symbol
	INTERFACE Globals
	INTERFACE Buffer
)