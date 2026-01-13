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

    void visitChildren(ASTVisitor &v) override {
        callee->accept(v);
        std::ranges::for_each(args, [&v](Expr* arg) { arg->accept(v); });
    }

    void accept(ASTVisitor &v) override {
        v.visit(this);
    }
};



#endif //CALLEXPR_H
