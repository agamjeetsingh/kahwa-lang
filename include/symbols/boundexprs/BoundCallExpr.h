//
// Created by Agamjeet Singh on 28/12/25.
//

#ifndef BOUNDCALLEXPR_H
#define BOUNDCALLEXPR_H
#include "BoundExpr.h"

// global functions and static methods
struct BoundCallExpr : BoundExpr {
    BoundCallExpr(FunctionSymbol* function, const std::vector<BoundExpr*>& args):
    BoundExpr(function->returnType, BoundExprKind::CALL_EXPR),
    function(function),
    args(args) {}

    FunctionSymbol* function;
    std::vector<BoundExpr*> args;
};



#endif //BOUNDCALLEXPR_H
