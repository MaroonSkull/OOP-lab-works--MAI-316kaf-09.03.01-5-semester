cmake_minimum_required (VERSION 3.8)
project (${PROJECT_NAME})


add_library (Line STATIC Line.cpp)
add_dependencies (${PROJECT_NAME} Line)

target_include_directories (Line PUBLIC ./)

target_compile_features (Line PUBLIC cxx_std_20)

target_link_libraries (Line
	PUBLIC Symbol
	PRIVATE Globals
	PRIVATE Buffer
)