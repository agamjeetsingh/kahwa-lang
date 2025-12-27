//
// Created by Agamjeet Singh on 26/12/25.
//

#ifndef MEMBERACCESSEXPR_H
#define MEMBERACCESSEXPR_H
#include <string>

#include "Expr.h"


struct MemberAccessExpr : Expr {
    MemberAccessExpr(Expr* base, const std::string& member, const SourceRange& bodyRange):
    Expr(bodyRange, ExprKind::MEMBER_ACCESS_EXPR), base(base), member(member) {}

    Expr* base;
    std::string member;
};




#endif //MEMBERACCESSEXPR_H
