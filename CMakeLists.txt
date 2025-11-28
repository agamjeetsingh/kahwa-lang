cmake_minimum_required(VERSION 3.31)
project(kahwa_lang)

set(CMAKE_CXX_STANDARD 20)

add_executable(kahwa_lang main.cpp
        src/tokeniser/Token.cpp
        include/tokeniser/Token.h
        include/tokeniser/TokenType.h
        src/tokeniser/Tokeniser.cpp
        include/tokeniser/Tokeniser.h
        src/parser/Class.cpp
        include/parser/Class.h
        include/parser/VisibilityModifier.h
        include/diagnostics/semantic/ModifierError.h
        include/parser/ExtensibilityModifier.h
        src/parser/exceptions/CompilationError.cpp
        include/diagnostics/CompilationError.h
        include/diagnostics/semantic/SemanticError.h
        include/diagnostics/lexical/LexicalError.h
        include/diagnostics/lexical/UnterminatedStringLiteralError.h
        include/diagnostics/lexical/UnrecognisedTokenError.h
        include/diagnostics/DiagnosticEngine.h
        include/diagnostics/DiagnosticKind.h
        include/diagnostics/Diagnostic.h
        src/source/SourceManager.cpp
        include/source/SourceManager.h
        include/source/SourceFile.h
        include/source/SourceLocation.h)
