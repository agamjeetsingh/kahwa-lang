//
// Created by Agamjeet Singh on 26/12/25.
//

#ifndef IFSTMT_H
#define IFSTMT_H
#include <optional>

#include "../expr/Expr.h"
#include "../expr/Stmt.h"


struct Block;

struct IfStmt : Stmt {
    IfStmt(Expr* cond, Block* ifBlock, Block* elseBlock, const SourceRange& bodyRange, const SourceRange &ifRange, std::optional<SourceRange> elseRange = std::nullopt):
    Stmt(bodyRange, StmtKind::IF),
    cond(cond),
    ifBlock(ifBlock),
    elseBlock(elseBlock),
    ifRange(ifRange),
    elseRange(elseRange) {}

    Expr* cond;
    Block* ifBlock;
    Block* elseBlock;
    SourceRange ifRange;
    std::optional<SourceRange> elseRange;
};



#endif //IFSTMT_H
