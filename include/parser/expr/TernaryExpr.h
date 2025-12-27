//
// Created by Agamjeet Singh on 26/12/25.
//

#ifndef TERNARYEXPR_H
#define TERNARYEXPR_H
#include "Expr.h"


struct TernaryExpr : Expr {
    Expr* cond;
    Expr* expr1;
    Expr* expr2;
};



#endif //TERNARYEXPR_H
