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
        include/parser/Modifier.h
        src/parser/FieldDecl.cpp
        include/parser/FieldDecl.h
        src/parser/MethodDecl.cpp
        include/parser/MethodDecl.h
        include/parser/TypeRef.h
        src/arena/Arena.cpp
        include/arena/Arena.h
        src/parser/Parser.cpp
        include/parser/Parser.h
        include/parser/TypedefDecl.h
        include/parser/KahwaFile.h
        src/source/SourceLocation.cpp
        src/source/SourceRange.cpp
        src/parser/Block.cpp
        include/parser/Block.h
        src/parser/Stmt.cpp
        include/parser/Stmt.h
        src/parser/Decl.cpp
        include/parser/Decl.h
        src/parser/ASTBuilder.cpp
        include/parser/ASTBuilder.h
        src/symbols/FunctionSymbol.cpp
        include/symbols/FunctionSymbol.h
        src/symbols/ClassSymbol.cpp
        include/symbols/ClassSymbol.h
        src/symbols/TypeParameterSymbol.cpp
        include/symbols/TypeParameterSymbol.h
        src/symbols/MethodSymbol.cpp
        include/symbols/MethodSymbol.h
        src/symbols/SemanticAnalyser.cpp
        include/symbols/SemanticAnalyser.h
        src/symbols/SymbolBuilder.cpp
        include/symbols/SymbolBuilder.h
        include/parser/ModifierNode.cpp
        include/parser/ModifierNode.h
        src/types/Type.cpp
        include/types/Type.h
        include/types/Variance.cpp
        include/types/Variance.h
        include/symbols/TypeSymbol.cpp
        include/symbols/TypeSymbol.h
        include/symbols/Symbol.cpp
        include/symbols/Symbol.h
)

target_link_libraries(kahwa_lang PRIVATE magic_enum::magic_enum)

enable_testing()

add_executable(
    tests
        tests/tokeniser/TokenTest.cpp
        tests/tokeniser/TokeniserTest.cpp
        tests/diagnostics/DiagnosticEngineTest.cpp
        tests/parser/ParserTest.cpp
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
        include/parser/Modifier.h
        src/parser/FieldDecl.cpp
        include/parser/FieldDecl.h
        src/parser/MethodDecl.cpp
        include/parser/MethodDecl.h
        include/parser/TypeRef.h
        src/arena/Arena.cpp
        include/arena/Arena.h
        src/parser/Parser.cpp
        include/parser/Parser.h
        include/parser/TypedefDecl.h
        include/parser/KahwaFile.h
        src/source/SourceLocation.cpp
        src/source/SourceRange.cpp
        src/parser/ASTBuilder.cpp
        include/parser/ASTBuilder.h
        src/symbols/FunctionSymbol.cpp
        include/symbols/FunctionSymbol.h
        src/symbols/ClassSymbol.cpp
        include/symbols/ClassSymbol.h
        src/symbols/TypeParameterSymbol.cpp
        include/symbols/TypeParameterSymbol.h
        src/symbols/MethodSymbol.cpp
        include/symbols/MethodSymbol.h
        src/symbols/SemanticAnalyser.cpp
        include/symbols/SemanticAnalyser.h
        src/symbols/SymbolBuilder.cpp
        include/symbols/SymbolBuilder.h
        tests/semantic_analysis/SemanticAnalyserTest.cpp
        tests/DiagnosticTesting.h
        tests/DiagnosticTesting.cpp
        tests/semantic_analysis/TypeTest.cpp
)

target_link_libraries(
    tests
    gtest_main
    magic_enum::magic_enum
)

include(GoogleTest)
gtest_discover_tests(tests)