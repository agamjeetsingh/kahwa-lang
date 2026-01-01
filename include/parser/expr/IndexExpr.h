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

    void accept(ASTVisitor &v) override {
        v.visit(this);
    }

    void visitChildren(ASTVisitor &v) override {
        callee->accept(v);
        arg->accept(v);
    }
};



#endif //INDEXEXPR_H
