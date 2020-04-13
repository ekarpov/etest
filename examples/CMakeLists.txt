cmake_minimum_required(VERSION 3.10)
project(etest_examples)

add_executable(etest_example etest_example.c)
target_link_libraries(etest_example PUBLIC libetest)
