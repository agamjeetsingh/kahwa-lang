//
// Created by Agamjeet Singh on 28/12/25.
//

#ifndef INDEXEXPR_H
#define INDEXEXPR_H
#include <vector>

#include "Expr.h"


struct IndexExpr : Expr {
    IndexExpr(Expr* callee, Expr* arg, const SourceRange& bodyRange):
    Expr(bodyRange, ExprKind::INDEX_EXPR),
    callee(callee),
    arg(arg) {}

    Expr* callee;
    Expr* arg;
};



#endif //INDEXEXPR_H
