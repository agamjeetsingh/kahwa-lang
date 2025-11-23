//
// Created by Agamjeet Singh on 22/11/25.
//

#include "../../include/tokeniser/Token.h"
#include <iostream>
#include <unordered_map>

std::string tokenTypeToString(TokenType type) {
    static const std::unordered_map<TokenType, std::string> tokenMap = {
        {TokenType::COLON, ":"},
        {TokenType::SEMI_COLON, ";"},
        {TokenType::COMMA, ","},
        {TokenType::LEFT_CURLY_BRACE, "{"},
        {TokenType::RIGHT_CURLY_BRACE, "}"},
        {TokenType::LEFT_PAREN, "("},
        {TokenType::RIGHT_PAREN, ")"},
        {TokenType::LEFT_BRACKET, "["},
        {TokenType::RIGHT_BRACKET, "]"},
        {TokenType::EQUALS, "="},
        {TokenType::DOUBLE_EQUALS, "=="},
        {TokenType::LESS, "<"},
        {TokenType::GREATER, ">"},
        {TokenType::LESS_EQUALS, "<="},
        {TokenType::GREATER_EQUALS, ">="},
        {TokenType::NOT, "!"},
        {TokenType::NOT_EQUALS, "!="},
        {TokenType::PLUS, "+"},
        {TokenType::MINUS, "-"},
        {TokenType::STAR, "*"},
        {TokenType::SLASH, "/"},
        {TokenType::MODULO, "%"},
        {TokenType::PLUS_EQUALS, "+="},
        {TokenType::MINUS_EQUALS, "-="},
        {TokenType::STAR_EQUALS, "*="},
        {TokenType::SLASH_EQUALS, "/="},
        {TokenType::MODULO_EQUALS, "%="},
        {TokenType::LEFT_SHIFT_EQUALS, "<<="},
        {TokenType::RIGHT_SHIFT_EQUALS, ">>="},
        {TokenType::BITWISE_AND_EQUALS, "&="},
        {TokenType::BITWISE_OR_EQUALS, "|="},
        {TokenType::BITWISE_XOR_EQUALS, "^="},
        {TokenType::INCREMENT, "++"},
        {TokenType::DECREMENT, "--"},
        {TokenType::LOGICAL_AND, "&&"},
        {TokenType::LOGICAL_OR, "||"},
        {TokenType::BITWISE_AND, "&"},
        {TokenType::BITWISE_OR, "|"},
        {TokenType::BITWISE_XOR, "^"},
        {TokenType::LEFT_SHIFT, "<<"},
        {TokenType::RIGHT_SHIFT, ">>"},
        {TokenType::QUESTION, "?"},
        {TokenType::DOT, "."},
        {TokenType::CLASS, "class"},
        {TokenType::STATIC, "static"},
        {TokenType::PUBLIC, "public"},
        {TokenType::PRIVATE, "private"},
        {TokenType::PROTECTED, "protected"},
        {TokenType::OPEN, "open"},
        {TokenType::FINAL, "final"},
        {TokenType::ABSTRACT, "abstract"},
        {TokenType::TYPEDEF, "typedef"},
        {TokenType::RETURN, "return"},
        {TokenType::IF, "if"},
        {TokenType::ELSE, "else"},
        {TokenType::FOR, "for"},
        {TokenType::WHILE, "while"},
        {TokenType::BREAK, "break"},
        {TokenType::CONTINUE, "continue"},
        {TokenType::TRUE, "true"},
        {TokenType::FALSE, "false"},
        {TokenType::NULL_LITERAL, "null"},
        {TokenType::IDENTIFIER, "IDENTIFIER"},
        {TokenType::STRING_LITERAL, "STRING"},
        {TokenType::INTEGER, "INTEGER"},
        {TokenType::FLOAT, "FLOAT"}
    };
    
    auto it = tokenMap.find(type);
    return it != tokenMap.end() ? it->second : "UNKNOWN";
}

std::string tokenToString(const Token& token) {
    if (const auto* str = token.getIf<std::string>()) {
        return "\"" + *str + "\"";
    }
    if (const auto* intVal = token.getIf<int>()) {
        return std::to_string(*intVal);
    }
    if (const auto* floatVal = token.getIf<float>()) {
        return std::to_string(*floatVal);
    }
    return tokenTypeToString(token.type);
}
