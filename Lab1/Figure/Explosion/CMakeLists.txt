cmake_minimum_required (VERSION 3.8)
project (${PROJECT_NAME})


add_library (Explosion STATIC Explosion.cpp)
add_dependencies (${PROJECT_NAME} Explosion)

target_include_directories (Explosion PUBLIC ./)

target_compile_features (Explosion PUBLIC cxx_std_20)

target_link_libraries (Explosion
	PUBLIC Figure
)