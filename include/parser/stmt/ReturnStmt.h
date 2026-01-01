//
// Created by Agamjeet Singh on 26/12/25.
//

#ifndef RETURNSTMT_H
#define RETURNSTMT_H
#include "../expr/Expr.h"
#include "../expr/Stmt.h"


struct ReturnStmt : Stmt {
    ReturnStmt(Expr* expr, const SourceRange& bodyRange, const SourceRange& returnRange):
    Stmt(bodyRange, StmtKind::RETURN), expr(expr), returnRange(returnRange) {}

    Expr* expr;
    SourceRange returnRange;

    void accept(ASTVisitor &v) override {
        v.visit(this);
    }

    void visitChildren(ASTVisitor &v) override {
        expr->accept(v);
    }
};



#endif //RETURNSTMT_H
