cmake_minimum_required(VERSION 3.10)
project(etest)

#file(GLOB_RECURSE SOURCES RELATIVE ${CMAKE_SOURCE_DIR} "src/*.c")
#add_library(${PROJECT_NAME} STATIC ${SOURCE})

# library
add_subdirectory(src)

# include examples only if this is main project
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    add_subdirectory(examples)
endif()