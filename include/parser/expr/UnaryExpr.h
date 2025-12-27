//
// Created by Agamjeet Singh on 26/12/25.
//

#ifndef UNARYEXPR_H
#define UNARYEXPR_H
#include "UnaryOp.h"
#include "Expr.h"


struct UnaryExpr : Expr {
    Expr* expr;
    UnaryOp op;
};



#endif //UNARYEXPR_H
