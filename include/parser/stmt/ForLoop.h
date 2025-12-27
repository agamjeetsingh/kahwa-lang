//
// Created by Agamjeet Singh on 26/12/25.
//

#ifndef FORLOOP_H
#define FORLOOP_H
#include "../expr/Expr.h"
#include "../expr/Stmt.h"


struct Block;

struct ForLoop : Stmt {
    Stmt* init;
    Expr* cond;
    Stmt* step;
    Block* body;
};



#endif //FORLOOP_H
