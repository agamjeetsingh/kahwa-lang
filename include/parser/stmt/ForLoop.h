//
// Created by Agamjeet Singh on 26/12/25.
//

#ifndef FORLOOP_H
#define FORLOOP_H
#include "../expr/Expr.h"
#include "../expr/Stmt.h"


struct Block;

struct ForLoop : Stmt {
    ForLoop(Stmt* init, Expr* cond, Stmt* step, Block* body, const SourceRange& bodyRange, const SourceRange &forRange):
    Stmt(bodyRange),
    init(init),
    cond(cond),
    step(step),
    body(body),
    forRange(forRange) {}

    Stmt* init;
    Expr* cond;
    Stmt* step;
    Block* body;
    SourceRange forRange;
};



#endif //FORLOOP_H
