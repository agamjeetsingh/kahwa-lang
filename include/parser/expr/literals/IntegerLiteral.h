//
// Created by Agamjeet Singh on 26/12/25.
//

#ifndef INTEGERLITERAL_H
#define INTEGERLITERAL_H
#include "../Expr.h"


struct IntegerLiteral : Expr {
    IntegerLiteral(float val, const SourceRange& bodyRange): Expr(bodyRange, ExprKind::INTEGER_LITERAL), val(val) {}

    int val;

    void accept(ASTVisitor &v) override {
        v.visit(this);
    }

    void visitChildren(ASTVisitor &v) override {}
};



#endif //INTEGERLITERAL_H
