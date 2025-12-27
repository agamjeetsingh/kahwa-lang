//
// Created by Agamjeet Singh on 26/12/25.
//

#ifndef BINARYOP_H
#define BINARYOP_H
#include <optional>

#include <magic_enum.hpp>
#include "../../tokeniser/TokenType.h"
#include "../../tokeniser/Token.h"

enum class BinaryOp {
    EQUALS, // "="
    DOUBLE_EQUALS, // "=="
    LESS, // "<"
    GREATER, // ">"
    LESS_EQUALS, // "<="
    GREATER_EQUALS, // ">="
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
    LOGICAL_AND, // "&&"
    LOGICAL_OR, // "||"
    BITWISE_AND, // "&"
    BITWISE_OR, // "|"
    BITWISE_XOR, // "^"
    LEFT_SHIFT, // "<<"
    RIGHT_SHIFT, // ">>"
};

inline std::optional<BinaryOp> tokenTypeToBinaryOp(TokenType type) {
    auto name = magic_enum::enum_name(type);
    if (name.empty()) return std::nullopt;

    return magic_enum::enum_cast<BinaryOp>(name);
}

inline TokenType binaryOpToTokenType(BinaryOp op) {
    auto name = magic_enum::enum_name(op);
    assert(!name.empty());

    return magic_enum::enum_cast<TokenType>(name).value();
}

inline std::string toString(BinaryOp op) {
    return tokenTypeToString(binaryOpToTokenType(op));
}


#endif //BINARYOP_H
