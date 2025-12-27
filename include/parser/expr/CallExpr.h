//
// Created by Agamjeet Singh on 26/12/25.
//

#ifndef CALLEXPR_H
#define CALLEXPR_H
#include <vector>

#include "Expr.h"


struct CallExpr : Expr {
    CallExpr(Expr* callee, const std::vector<Expr*>& args, const SourceRange& bodyRange):
    Expr(bodyRange, ExprKind::CALL_EXPR),
    callee(callee),
    args(args) {}

    Expr* callee;
    std::vector<Expr*> args;
};



#endif //CALLEXPR_H
