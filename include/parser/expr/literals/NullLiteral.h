//
// Created by Agamjeet Singh on 26/12/25.
//

#ifndef NULLLITERAL_H
#define NULLLITERAL_H
#include "../Expr.h"


struct NullLiteral : Expr {
    NullLiteral(const SourceRange& bodyRange): Expr(bodyRange, ExprKind::NULL_LITERAL) {}
};



#endif //NULLLITERAL_H
