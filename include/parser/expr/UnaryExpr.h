//
// Created by Agamjeet Singh on 26/12/25.
//

#ifndef UNARYEXPR_H
#define UNARYEXPR_H
#include "UnaryOp.h"
#include "Expr.h"


struct UnaryExpr : Expr {
    UnaryExpr(Expr* expr, UnaryOp op, const SourceRange& bodyRange):
    Expr(bodyRange, ExprKind::UNARY_EXPR), expr(expr), op(op) {}

    Expr* expr;
    UnaryOp op;
};



#endif //UNARYEXPR_H
