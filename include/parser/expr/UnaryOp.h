//
// Created by Agamjeet Singh on 26/12/25.
//

#ifndef UNARYOP_H
#define UNARYOP_H

#include <magic_enum.hpp>

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
