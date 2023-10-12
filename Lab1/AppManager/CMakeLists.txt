cmake_minimum_required (VERSION 3.8)
project (${PROJECT_NAME})


add_library (AppManager STATIC AppManager.cpp)
add_dependencies (${PROJECT_NAME} AppManager)

target_include_directories (AppManager PUBLIC ./)

target_compile_features (AppManager PUBLIC cxx_std_20)

target_link_libraries (AppManager PUBLIC Line)