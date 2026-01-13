//
// Created by Agamjeet Singh on 26/12/25.
//

#ifndef MEMBERACCESSEXPR_H
#define MEMBERACCESSEXPR_H
#include <string>

#include "Expr.h"


struct MemberAccessExpr : Expr {
    MemberAccessExpr(Expr* base, const std::string& member, const SourceRange& bodyRange, const SourceRange& memberNameRange):
    Expr(bodyRange, ExprKind::MEMBER_ACCESS_EXPR), base(base), member(member), memberNameRange(memberNameRange) {}

    Expr* base;
    std::string member;
    SourceRange memberNameRange;

    void accept(ASTVisitor &v) override {
        v.visit(this);
    }

    void visitChildren(ASTVisitor &v) override {
        base->accept(v);
    }
};




#endif //MEMBERACCESSEXPR_H
