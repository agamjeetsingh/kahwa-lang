//
// Created by Agamjeet Singh on 26/12/25.
//

#ifndef WHILELOOP_H
#define WHILELOOP_H
#include "../expr/Expr.h"
#include "../expr/Stmt.h"


struct Block;

struct WhileLoop : Stmt {
    WhileLoop(Expr* cond, Block* body, const SourceRange& bodyRange, const SourceRange& whileRange):
    Stmt(bodyRange, StmtKind::WHILE), cond(cond), body(body), whileRange(whileRange) {}

    Expr* cond;
    Block* body;
    SourceRange whileRange;
};



#endif //WHILELOOP_H
