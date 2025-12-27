//
// Created by Agamjeet Singh on 26/12/25.
//

#ifndef EXPR_H
#define EXPR_H
#include "ExprKind.h"
#include "Stmt.h"
#include "../../source/SourceRange.h"


struct Expr {
    Expr(const SourceRange& bodyRange, ExprKind kind = ExprKind::EXPR): bodyRange(bodyRange), kind(kind) {}

    virtual ~Expr() = default;

    const SourceRange bodyRange;
    const ExprKind kind;
};



#endif //EXPR_H
