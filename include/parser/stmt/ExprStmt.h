//
// Created by Agamjeet Singh on 27/12/25.
//

#ifndef EXPRSTMT_H
#define EXPRSTMT_H
#include "../expr/Stmt.h"
#include "../expr/Expr.h"


struct ExprStmt : Stmt {
    ExprStmt(Expr* expr, const SourceRange& bodyRange): Stmt(bodyRange, StmtKind::EXPR), expr(expr) {}
    Expr* expr;

    void accept(ASTVisitor &v) override {
        v.visit(this);
    }

    void visitChildren(ASTVisitor &v) override {
        expr->accept(v);
    }
};



#endif //EXPRSTMT_H
