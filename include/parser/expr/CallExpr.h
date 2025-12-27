//
// Created by Agamjeet Singh on 26/12/25.
//

#ifndef CALLEXPR_H
#define CALLEXPR_H
#include <vector>

#include "Expr.h"


struct CallExpr : Expr {
    Expr* callee;
    std::vector<Expr*> args;
};



#endif //CALLEXPR_H
