cmake_minimum_required(VERSION 3.0.0)
project(compiler VERSION 0.1.0)

include(CTest)
enable_testing()

add_executable(compiler main.c error_handler.h error_handler.c code_parser.h code_parser.c writer.h writer.c)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

target_link_libraries(compiler -lm)

add_compile_options(-Wall -Wextra -pedantic -Werror)
