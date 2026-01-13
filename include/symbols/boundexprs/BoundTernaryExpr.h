//
// Created by Agamjeet Singh on 28/12/25.
//

#ifndef BOUNDTERNARYEXPR_H
#define BOUNDTERNARYEXPR_H
#include "BoundExpr.h"


struct BoundTernaryExpr : BoundExpr {
    BoundTernaryExpr(BoundExpr* cond, BoundExpr* expr1, BoundExpr* expr2, Type* type)
    : BoundExpr(type, BoundExprKind::TERNARY_EXPR),
    cond(cond),
    expr1(expr1),
    expr2(expr2) {}

    BoundExpr* cond;
    BoundExpr* expr1;
    BoundExpr* expr2;
};



#endif //BOUNDTERNARYEXPR_H
