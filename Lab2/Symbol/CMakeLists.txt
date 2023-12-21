cmake_minimum_required (VERSION 3.8)
project (${PROJECT_NAME})


add_library (Symbol STATIC Symbol.cpp)
add_dependencies (${PROJECT_NAME} Symbol)

target_include_directories (Symbol PUBLIC ./)

target_link_libraries (Symbol
	PUBLIC Globals
	PUBLIC Buffer
)

target_compile_features (Symbol PUBLIC cxx_std_20)