cmake_minimum_required(VERSION 3.10)
project(libetest)

# files
file(GLOB_RECURSE SOURCES RELATIVE ${CMAKE_SOURCE_DIR}/src "*.c")
file(GLOB_RECURSE HEADERS RELATIVE ${CMAKE_SOURCE_DIR}/src "*.h")

add_library(${PROJECT_NAME} STATIC ${SOURCES} ${HEADERS})

target_include_directories(libetest PUBLIC ${CMAKE_SOURCE_DIR}/include)