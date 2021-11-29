cmake_minimum_required(VERSION 3.20)
project(C_compiler)

set(CMAKE_CXX_STANDARD 14)

add_executable(C_compiler lexer.cpp lexer.h parser.cpp parser.h evaluator.cpp evaluator.h main.cpp error.h error.cpp)
