//
// Created by Agamjeet Singh on 26/12/25.
//

#ifndef STRINGLITERAL_H
#define STRINGLITERAL_H
#include <string>

#include "../Expr.h"


struct StringLiteral : Expr {
    StringLiteral(std::string val, const SourceRange& bodyRange): Expr(bodyRange, ExprKind::STRING_LITERAL), val(val) {}

    std::string val;

    void accept(ASTVisitor &v) override {
        v.visit(this);
    }

    void visitChildren(ASTVisitor &v) override {}
};



#endif //STRINGLITERAL_H
