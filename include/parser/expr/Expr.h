//
// Created by Agamjeet Singh on 26/12/25.
//

#ifndef EXPR_H
#define EXPR_H
#include "ExprKind.h"
#include "Stmt.h"
#include "../../source/SourceRange.h"


struct Expr : ASTNode {
    explicit Expr(const SourceRange& bodyRange, ExprKind kind = ExprKind::EXPR): ASTNode(bodyRange), kind(kind) {}

    const ExprKind kind;
};



#endif //EXPR_H
