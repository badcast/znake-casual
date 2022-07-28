cmake_minimum_required(VERSION 2.0)

project(ZnakeCasual LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

#get an roninel - ronin-engine-library
add_subdirectory(RoninEngine)

file(GLOB_RECURSE ZNAKE_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp")

file(GLOB_RECURSE ZNAKE_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/source/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/source/*.h")


add_executable(ZnakeCasual ${ZNAKE_HEADERS} ${ZNAKE_SOURCES})
set_target_properties(ZnakeCasual PROPERTIES OUTPUT_NAME znakegame)

target_link_libraries(ZnakeCasual PUBLIC RoninEngine)
