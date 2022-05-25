#get max version CMAKE
cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

project("NightFear" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL2 REQUIRED)

message("${PROJECT_NAME}")

#FUNCTIONS
include(cmake/functions.cmake)

set(RUNTIME_OUTPUT_NAME "NightFear")

if(UNIX)
    set(OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build/linux/)

    # custom_log("<This is Linux Runtime>")
    message("<This is Linux Runtime>")
elseif(WIN32)
    set(OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build/windows/)

    # custom_log("<This is Windows Runtime>")
    message("<This is Windows Runtime>")
endif()

SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${OUTPUT_DIRECTORY}")
SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIRECTORY}")
SET( CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${OUTPUT_DIRECTORY}")
SET( CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIRECTORY}")
SET( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${OUTPUT_DIRECTORY}")
SET( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIRECTORY}")

add_subdirectory(Sources) #get sources and linking from the "Sources" directory
