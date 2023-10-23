# CMakeList.txt : CMake project for Lab1, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.12)

project ("Lab1"
    VERSION 0.1
    DESCRIPTION "Решение первой лабораторной работы по предмету Объектно-ориентированное программирование"
    HOMEPAGE_URL "https://lms.mai.ru/course/view.php?id=2515"
    LANGUAGES CXX
)

# Add source to this project's executable.
add_executable (${PROJECT_NAME} "Lab1.cpp")
target_compile_features (${PROJECT_NAME} PUBLIC cxx_std_17)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

add_library(Globals INTERFACE)
add_dependencies (${PROJECT_NAME} Globals)

target_include_directories(Globals INTERFACE ./)

add_subdirectory ("Buffer")
add_subdirectory ("Symbol")
add_subdirectory ("Line")
add_subdirectory ("AppManager")

# link libraries
foreach (library
    Buffer
    AppManager
    Line
    Symbol
    Globals
)
    target_link_libraries (${PROJECT_NAME} PRIVATE ${library})
endforeach ()

# TODO: Add tests and install targets if needed.
