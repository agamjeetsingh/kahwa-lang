//
// Created by Agamjeet Singh on 26/12/25.
//

#ifndef INTEGERLITERAL_H
#define INTEGERLITERAL_H
#include "../Expr.h"


struct IntegerLiteral : Expr {
    IntegerLiteral(float val, const SourceRange& bodyRange): Expr(bodyRange), val(val) {}

    int val;
};



#endif //INTEGERLITERAL_H
