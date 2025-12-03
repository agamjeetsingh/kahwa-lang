cmake_minimum_required(VERSION 3.31)
project(kahwa_lang)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)

FetchContent_Declare(
        magic_enum
        GIT_REPOSITORY https://github.com/Neargye/magic_enum.git
        GIT_TAG        v0.9.5   # or latest release
)

FetchContent_MakeAvailable(magic_enum)

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        release-1.12.1
)
FetchContent_MakeAvailable(googletest)

add_executable(kahwa_lang main.cpp
        src/tokeniser/Token.cpp
        include/tokeniser/Token.h
        include/tokeniser/TokenType.h
        src/tokeniser/Tokeniser.cpp
        include/tokeniser/Tokeniser.h
        src/parser/ClassDecl.cpp
        include/parser/ClassDecl.h
        include/diagnostics/DiagnosticEngine.h
        include/diagnostics/DiagnosticSeverity.h
        include/diagnostics/Diagnostic.h
        src/source/SourceManager.cpp
        include/source/SourceManager.h
        include/source/SourceFile.h
        include/source/SourceLocation.h
        include/diagnostics/DiagnosticKind.h
        include/source/SourceRange.h
        include/parser/Modifier.h)
        include/arena/Arena.h
        src/source/SourceLocation.cpp
        src/source/SourceRange.cpp
)

target_link_libraries(kahwa_lang PRIVATE magic_enum::magic_enum)

enable_testing()

add_executable(
    tests
        tests/tokeniser/TokenTest.cpp
    src/tokeniser/Token.cpp
    include/tokeniser/Token.h
    include/tokeniser/TokenType.h
    include/source/SourceRange.h
    include/source/SourceLocation.h
        src/tokeniser/Tokeniser.cpp
        include/tokeniser/Tokeniser.h
        tests/tokeniser/TokeniserTest.cpp
)

target_link_libraries(
    tests
    gtest_main
    magic_enum::magic_enum
)

include(GoogleTest)
gtest_discover_tests(tests)