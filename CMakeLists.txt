cmake_minimum_required(VERSION 3.16)
project(FinalProjectDSA LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

include(FetchContent)
FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2.6.x)
FetchContent_MakeAvailable(SFML)

add_executable(FinalProjectDSA main.cpp
        helper.h
        programs.h
        dropdown.h)
target_link_libraries(FinalProjectDSA PRIVATE sfml-graphics)
target_compile_features(FinalProjectDSA PRIVATE cxx_std_17)

