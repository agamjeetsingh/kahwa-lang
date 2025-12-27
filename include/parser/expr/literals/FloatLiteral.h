//
// Created by Agamjeet Singh on 26/12/25.
//

#ifndef FLOATLITERAL_H
#define FLOATLITERAL_H
#include "../Expr.h"


struct FloatLiteral : Expr {
    FloatLiteral(float val, const SourceRange& bodyRange): Expr(bodyRange, ExprKind::FLOAT_LITERAL), val(val) {}

    float val;
};



#endif //FLOATLITERAL_H
