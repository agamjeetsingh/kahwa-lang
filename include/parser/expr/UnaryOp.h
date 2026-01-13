//
// Created by Agamjeet Singh on 26/12/25.
//

#ifndef UNARYOP_H
#define UNARYOP_H

#include <magic_enum.hpp>

#include "../../tokeniser/TokenType.h"

enum class TokenType;

// TODO - Possibly missing bitwise negation ~x
enum class UnaryOp {
    NOT, // "!"
    PLUS, // "+"
    MINUS, // "-"
    POST_INCREMENT, // "++"
    POST_DECREMENT, // "--"
    PRE_INCREMENT, // "++"
    PRE_DECREMENT, // "--"
};

inline UnaryOp tokenTypeToPrefixUnaryOp(const TokenType type) {
    switch (type) {
        case TokenType::NOT:
            return UnaryOp::NOT;
        case TokenType::PLUS:
            return UnaryOp::PLUS;
        case TokenType::MINUS:
            return UnaryOp::MINUS;
        case TokenType::INCREMENT:
            return UnaryOp::PRE_INCREMENT;
        case TokenType::DECREMENT:
            return UnaryOp::PRE_DECREMENT;
        default: throw std::invalid_argument("Token type cannot be converted to prefix unitary operator.");
    }
}

inline UnaryOp tokenTypeToPostfixUnaryOp(const TokenType type) {
    switch (type) {
        case TokenType::NOT:
            return UnaryOp::NOT;
        case TokenType::PLUS:
            return UnaryOp::PLUS;
        case TokenType::MINUS:
            return UnaryOp::MINUS;
        case TokenType::INCREMENT:
            return UnaryOp::POST_INCREMENT;
        case TokenType::DECREMENT:
            return UnaryOp::POST_DECREMENT;
        default: throw std::invalid_argument("Token type cannot be converted to postfix unitary operator.");
    }
}

inline std::string toString(const UnaryOp op) {
    switch (op) {
        case UnaryOp::NOT:
            return "!";
        case UnaryOp::PLUS:
            return "+";
        case UnaryOp::MINUS:
            return "-";
        case UnaryOp::POST_INCREMENT:
        case UnaryOp::PRE_INCREMENT:
            return "++";
        case UnaryOp::POST_DECREMENT:
        case UnaryOp::PRE_DECREMENT:
            return "--";
    }
}

#endif //UNARYOP_H
