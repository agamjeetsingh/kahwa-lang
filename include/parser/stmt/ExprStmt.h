//
// Created by Agamjeet Singh on 27/12/25.
//

#ifndef EXPRSTMT_H
#define EXPRSTMT_H
#include "../expr/Stmt.h"


struct Expr;

struct ExprStmt : Stmt {
    ExprStmt(Expr* expr, const SourceRange& bodyRange): Stmt(bodyRange), expr(expr) {}
    Expr* expr;
};



#endif //EXPRSTMT_H
