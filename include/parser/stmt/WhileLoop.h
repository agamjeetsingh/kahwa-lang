//
// Created by Agamjeet Singh on 26/12/25.
//

#ifndef WHILELOOP_H
#define WHILELOOP_H
#include "../expr/Expr.h"
#include "../expr/Stmt.h"
#include "../Block.h"


struct WhileLoop : Stmt {
    WhileLoop(Expr* cond, Block* body, const SourceRange& bodyRange, const SourceRange& whileRange):
    Stmt(bodyRange, StmtKind::WHILE), cond(cond), body(body), whileRange(whileRange) {}

    Expr* cond;
    Block* body;
    SourceRange whileRange;

    void accept(ASTVisitor &v) override {
        v.visit(this);
    }

    void visitChildren(ASTVisitor &v) override {
        cond->accept(v);
        body->accept(v);
    }
};



#endif //WHILELOOP_H
