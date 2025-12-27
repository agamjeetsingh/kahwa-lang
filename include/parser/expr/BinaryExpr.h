//
// Created by Agamjeet Singh on 26/12/25.
//

#ifndef BINARYEXPR_H
#define BINARYEXPR_H
#include "BinaryOp.h"
#include "Expr.h"


struct BinaryExpr : Expr {
    BinaryExpr(Expr* expr1, Expr* expr2, BinaryOp op, const SourceRange& bodyRange):
    Expr(bodyRange), expr1(expr1), expr2(expr2), op(op) {}

    Expr* expr1;
    Expr* expr2;
    BinaryOp op;
};



#endif //BINARYEXPR_H
