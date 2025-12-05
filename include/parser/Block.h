//
// Created by Agamjeet Singh on 04/12/25.
//

#ifndef BLOCK_H
#define BLOCK_H
#include <vector>

#include "Stmt.h"


struct Block {
    explicit Block(const std::vector<Stmt*>& stmts): stmts(stmts) {}
    const std::vector<Stmt*> stmts;

    bool operator==(const Block &other) const {
        if (stmts.size() != other.stmts.size()) return false;
        
        for (size_t i = 0; i < stmts.size(); ++i) {
            if (stmts[i] == nullptr && other.stmts[i] == nullptr) continue;
            if (stmts[i] == nullptr || other.stmts[i] == nullptr) return false;
            if (!(*stmts[i] == *other.stmts[i])) return false;
        }
        
        return true;
    }
};



#endif //BLOCK_H
