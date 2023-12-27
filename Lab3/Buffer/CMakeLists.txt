cmake_minimum_required (VERSION 3.8)
project (${PROJECT_NAME})


add_library (Buffer STATIC Buffer.cpp)
add_dependencies (${PROJECT_NAME} Buffer)

target_include_directories (Buffer PUBLIC ./)

target_link_libraries (Buffer PUBLIC Globals)

target_compile_features (Buffer PUBLIC cxx_std_23)