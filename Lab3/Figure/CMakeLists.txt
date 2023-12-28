cmake_minimum_required (VERSION 3.8)
project (${PROJECT_NAME})


add_library (Figure INTERFACE Figure.hpp)
add_dependencies (${PROJECT_NAME} Figure)

target_include_directories (Figure INTERFACE ./)

target_compile_features (Figure INTERFACE cxx_std_20)

target_link_libraries (Figure
	INTERFACE Containers
	INTERFACE Symbol
	INTERFACE Globals
	INTERFACE Buffer
)

add_subdirectory ("Line")
add_subdirectory ("Explosion")