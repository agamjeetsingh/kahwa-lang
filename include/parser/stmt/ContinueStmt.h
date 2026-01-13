//
// Created by Agamjeet Singh on 26/12/25.
//

#ifndef CONTINUESTMT_H
#define CONTINUESTMT_H
#include "../expr/Stmt.h"


struct ContinueStmt : Stmt {
    explicit ContinueStmt(const SourceRange& bodyRange): Stmt(bodyRange, StmtKind::CONTINUE) {}

    void accept(ASTVisitor &v) override {
        v.visit(this);
    }

    void visitChildren(ASTVisitor &v) override {}
};



#endif //CONTINUESTMT_H
