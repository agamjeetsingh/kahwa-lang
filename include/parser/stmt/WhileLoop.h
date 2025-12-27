//
// Created by Agamjeet Singh on 26/12/25.
//

#ifndef WHILELOOP_H
#define WHILELOOP_H
#include "../expr/Expr.h"
#include "../expr/Stmt.h"


struct Block;

struct WhileLoop : Stmt {
    Expr* cond;
    Block* body;
};



#endif //WHILELOOP_H
