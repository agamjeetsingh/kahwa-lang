//
// Created by Agamjeet Singh on 04/12/25.
//

#ifndef BLOCK_H
#define BLOCK_H
#include <vector>

#include "Stmt.h"


class Block {
public:
    explicit Block(const std::vector<Stmt>& stmts): stmts(stmts) {}
private:
    std::vector<Stmt> stmts;
};



#endif //BLOCK_H
