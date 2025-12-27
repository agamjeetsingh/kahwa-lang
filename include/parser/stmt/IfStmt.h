//
// Created by Agamjeet Singh on 26/12/25.
//

#ifndef IFSTMT_H
#define IFSTMT_H
#include "../expr/Expr.h"
#include "../expr/Stmt.h"


struct Block;

struct IfStmt : Stmt {
    Expr* cond;
    Block* ifBlock;
    Block* elseBlock;
};



#endif //IFSTMT_H
