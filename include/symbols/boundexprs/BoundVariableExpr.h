//
// Created by Agamjeet Singh on 28/12/25.
//

#ifndef BOUNDVARIABLEEXPR_H
#define BOUNDVARIABLEEXPR_H
#include "BoundExpr.h"


struct BoundVariableExpr : BoundExpr {
    explicit BoundVariableExpr(VariableSymbol* variable):
    BoundExpr(variable->type), variable(variable) {}

    VariableSymbol* variable;
};



#endif //BOUNDVARIABLEEXPR_H
