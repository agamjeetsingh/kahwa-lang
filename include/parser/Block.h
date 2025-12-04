//
// Created by Agamjeet Singh on 04/12/25.
//

#ifndef BLOCK_H
#define BLOCK_H
#include <vector>

#include "Stmt.h"


struct Block {
    explicit Block(const std::vector<Stmt>& stmts): stmts(stmts) {}
    const std::vector<Stmt> stmts;
};



#endif //BLOCK_H
