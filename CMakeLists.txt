cmake_minimum_required(VERSION 3.5)

project(visa-data)

find_library(VISA NAMES VISA REQUIRED)
include_directories(${VISA}/Headers)
add_executable(main main.c parse_args.c)
target_link_libraries(main ${VISA})
