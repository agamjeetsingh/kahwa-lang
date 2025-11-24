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
        include/parser/exceptions/semantic/ModifierError.h
        include/parser/ExtensibilityModifier.h
        src/parser/exceptions/CompilationError.cpp
        include/parser/exceptions/CompilationError.h
        include/parser/exceptions/semantic/SemanticError.h
        include/parser/exceptions/lexical/LexicalError.h
        include/parser/exceptions/lexical/UnterminatedStringLiteralError.h
        include/parser/exceptions/lexical/UnrecognisedTokenError.h)
