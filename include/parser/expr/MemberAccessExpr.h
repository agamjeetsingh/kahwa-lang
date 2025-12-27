//
// Created by Agamjeet Singh on 26/12/25.
//

#ifndef MEMBERACCESSEXPR_H
#define MEMBERACCESSEXPR_H
#include <string>

#include "Expr.h"


struct MemberAccessExpr : Expr {
    Expr* base;
    std::string member;
};




#endif //MEMBERACCESSEXPR_H
