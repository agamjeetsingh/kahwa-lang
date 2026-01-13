//
// Created by Agamjeet Singh on 28/12/25.
//

#ifndef BOUNDEXPR_H
#define BOUNDEXPR_H
#include "BoundExprKind.h"
#include "../../parser/expr/ExprKind.h"
#include "../../types/Type.h"


struct BoundExpr {
    explicit BoundExpr(const Type* type, BoundExprKind kind = BoundExprKind::EXPR): type(type), kind(kind) {}

    virtual ~BoundExpr() = default;

    const Type* type;
    const BoundExprKind kind;
};



#endif //BOUNDEXPR_H
