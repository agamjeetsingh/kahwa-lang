//
// Created by Agamjeet Singh on 26/12/25.
//

#ifndef TERNARYEXPR_H
#define TERNARYEXPR_H
#include "Expr.h"


struct TernaryExpr : Expr {
    TernaryExpr(Expr* cond, Expr* expr1, Expr* expr2, const SourceRange& bodyRange):
    Expr(bodyRange, ExprKind::TERNARY_EXPR), cond(cond), expr1(expr1), expr2(expr2) {}

    Expr* cond;
    Expr* expr1;
    Expr* expr2;
};



#endif //TERNARYEXPR_H
