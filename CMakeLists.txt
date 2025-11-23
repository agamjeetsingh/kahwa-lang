cmake_minimum_required(VERSION 3.31)
project(kahwa_lang)

set(CMAKE_CXX_STANDARD 20)

add_executable(kahwa_lang main.cpp
        src/tokeniser/Token.cpp
        include/Token.h
        include/TokenType.h
        src/tokeniser/Tokeniser.cpp
        include/Tokeniser.h)
