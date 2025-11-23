//
// Created by Agamjeet Singh on 22/11/25.
//

#ifndef TOKENTYPE_H
#define TOKENTYPE_H

#endif //TOKENTYPE_H

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
    INTEGER,
    FLOAT,
};