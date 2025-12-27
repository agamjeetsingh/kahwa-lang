//
// Created by Agamjeet Singh on 26/12/25.
//

#ifndef BINARYOP_H
#define BINARYOP_H



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



#endif //BINARYOP_H
