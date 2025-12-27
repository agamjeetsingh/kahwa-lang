//
// Created by Agamjeet Singh on 27/12/25.
//

#ifndef EXPRKIND_H
#define EXPRKIND_H



enum class ExprKind {
    BOOL_LITERAL,
    FLOAT_LITERAL,
    INTEGER_LITERAL,
    NULL_LITERAL,
    STRING_LITERAL,
    BINARY_EXPR,
    CALL_EXPR,
    IDENTIFIER_REF,
    MEMBER_ACCESS_EXPR,
    TERNARY_EXPR,
    UNARY_EXPR,
    EXPR,
};



#endif //EXPRKIND_H
