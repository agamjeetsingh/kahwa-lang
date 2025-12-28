//
// Created by Agamjeet Singh on 04/12/25.
//

#ifndef BLOCK_H
#define BLOCK_H
#include <vector>
#include "ASTBuilder.h"
#include "expr/Stmt.h"


struct Block : Stmt {
    explicit Block(const std::vector<Stmt*>& stmts, const SourceRange& bodyRange): Stmt(bodyRange, StmtKind::BLOCK), stmts(stmts) {}
    const std::vector<Stmt*> stmts;

    bool operator==(const Block &other) const {
        if (stmts.size() != other.stmts.size()) return false;
        
        for (size_t i = 0; i < stmts.size(); ++i) {
            if (stmts[i] == nullptr && other.stmts[i] == nullptr) continue;
            if (stmts[i] == nullptr || other.stmts[i] == nullptr) return false;
            if (*stmts[i] != *other.stmts[i]) return false;
        }
        
        return true;
    }
};

class BlockBuilder : public ASTBuilder {
public:

    BlockBuilder() = default;

    [[nodiscard]] Block* build() const {
        return arena->make<Block>(stmts, dummy_source);
    }

    BlockBuilder& with(Stmt* stmt) {
        stmts.push_back(stmt);
        return *this;
    }

    BlockBuilder& with(const std::vector<Stmt*> &stmts) {
        this->stmts.insert(this->stmts.begin(), stmts.begin(), stmts.end());
        return *this;
    }

private:
    std::vector<Stmt*> stmts;
};


#endif //BLOCK_H
