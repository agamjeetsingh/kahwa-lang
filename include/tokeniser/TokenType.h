//
// Created by Agamjeet Singh on 22/11/25.
//

#ifndef TOKENTYPE_H
#define TOKENTYPE_H

#include <magic_enum.hpp>
#include <unordered_set>

enum class TokenType {
    COLON, // ":"
    SEMI_COLON, // ";"
    COMMA, // ","
    LEFT_CURLY_BRACE, // "{"
    RIGHT_CURLY_BRACE, // "}"
    LEFT_PAREN, // "("
    RIGHT_PAREN, // ")"
    LEFT_BRACKET, // "["
    RIGHT_BRACKET, // "]"

    EQUALS, // "="
    DOUBLE_EQUALS, // "=="
    LESS, // "<"
    GREATER, // ">"
    LESS_EQUALS, // "<="
    GREATER_EQUALS, // ">="
    NOT, // "!"
    NOT_EQUALS, // "!="
    PLUS, // "+"
    MINUS, // "-"
    STAR, // "*"
    SLASH, // "/"
    MODULO, // "%"
    PLUS_EQUALS, // "+="
    MINUS_EQUALS, // "-="
    STAR_EQUALS, // "*="
    SLASH_EQUALS, // "/="
    MODULO_EQUALS, // "%="
    LEFT_SHIFT_EQUALS, // "<<="
    RIGHT_SHIFT_EQUALS, // ">>="
    BITWISE_AND_EQUALS, // "&="
    BITWISE_OR_EQUALS, // "|="
    BITWISE_XOR_EQUALS, // "^="
    INCREMENT, // "++"
    DECREMENT, // "--"
    LOGICAL_AND, // "&&"
    LOGICAL_OR, // "||"
    BITWISE_AND, // "&"
    BITWISE_OR, // "|"
    BITWISE_XOR, // "^"
    LEFT_SHIFT, // "<<"
    RIGHT_SHIFT, // ">>"
    QUESTION, // "?"
    DOT, // "."

    CLASS, // "class"
    STATIC, // "static"
    PUBLIC, // "public"
    PRIVATE, // "private"
    PROTECTED, // "protected"
    OPEN, // "open"
    FINAL, // "final"
    ABSTRACT, // "abstract"
    INTERFACE, // "interface"
    TYPEDEF, // "typedef"
    IN, // "in"
    OUT, // "out"

    RETURN, // "return"
    IF, // "if"
    ELSE, // "else"
    FOR, // "for"
    WHILE, // "while"
    BREAK, // "break"
    CONTINUE, // "continue"

    TRUE, // "true"
    FALSE, // "false"
    NULL_LITERAL, // "null"

    IDENTIFIER,
    STRING_LITERAL,
    CHAR_LITERAL, // TODO()
    INTEGER,
    FLOAT,

    BAD,
};

inline const auto KEYWORD_TYPES = std::unordered_set{
    TokenType::CLASS, // "class"
    TokenType::STATIC, // "static"
    TokenType::PUBLIC, // "public"
    TokenType::PRIVATE, // "private"
    TokenType::PROTECTED, // "protected"
    TokenType::OPEN, // "open"
    TokenType::FINAL, // "final"
    TokenType::ABSTRACT, // "abstract"
    TokenType::INTERFACE, // "interface"
    TokenType::TYPEDEF, // "typedef"
    TokenType::IN, // "in"
    TokenType::OUT, // "out"

    TokenType::RETURN, // "return"
    TokenType::IF, // "if"
    TokenType::ELSE, // "else"
    TokenType::FOR, // "for"
    TokenType::WHILE, // "while"
    TokenType::BREAK, // "break"
    TokenType::CONTINUE, // "continue"

    TokenType::TRUE, // "true"
    TokenType::FALSE, // "false"
    TokenType::NULL_LITERAL, // "null"
};

inline const auto MODIFIER_TYPES = std::unordered_set{
    TokenType::STATIC, // "static"
    TokenType::PUBLIC, // "public"
    TokenType::PRIVATE, // "private"
    TokenType::PROTECTED, // "protected"
    TokenType::OPEN, // "open"
    TokenType::FINAL, // "final"
    TokenType::ABSTRACT, // "abstract"
};

inline std::string keywordToString(TokenType tokenType) {
    if (!KEYWORD_TYPES.contains(tokenType)) return "Not a keyword";
    if (tokenType == TokenType::NULL_LITERAL) return "null";
    std::string s = std::string{magic_enum::enum_name(tokenType)};
    std::ranges::transform(s, s.begin(),
                           [](unsigned char c){ return std::tolower(c); });
    return s;
}


#endif //TOKENTYPE_H