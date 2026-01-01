//
// Created by Agamjeet Singh on 04/12/25.
//

#ifndef STMT_H
#define STMT_H
#include "../../source/SourceRange.h"
#include "../stmt/StmtKind.h"
#include "../ASTNode.h"

struct Stmt : ASTNode {
    explicit Stmt(const SourceRange& bodyRange, const StmtKind kind = StmtKind::STMT): ASTNode(bodyRange), kind(kind) {}

    bool operator==(const Stmt &other) const {
        return true; // Empty struct, all instances are equal
    }

    const StmtKind kind;
};



#endif //STMT_H
