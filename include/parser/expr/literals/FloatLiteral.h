//
// Created by Agamjeet Singh on 26/12/25.
//

#ifndef FLOATLITERAL_H
#define FLOATLITERAL_H
#include "../Expr.h"


struct FloatLiteral : Expr {
    FloatLiteral(float val, const SourceRange& bodyRange): Expr(bodyRange, ExprKind::FLOAT_LITERAL), val(val) {}

    float val;

    void accept(ASTVisitor &v) override {
        v.visit(this);
    }

    void visitChildren(ASTVisitor &v) override {}
};



#endif //FLOATLITERAL_H
