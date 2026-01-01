//
// Created by Agamjeet Singh on 31/12/25.
//

#ifndef ASTNODE_H
#define ASTNODE_H

#include "ASTVisitor.h"
#include "../source/SourceRange.h"

struct ASTNode {
    explicit ASTNode(const SourceRange& bodyRange): bodyRange(bodyRange) {}

    const SourceRange bodyRange;

    virtual ~ASTNode() noexcept = default;
    virtual void accept(ASTVisitor& v) = 0;
    virtual void visitChildren(ASTVisitor& v) = 0;
};



#endif //ASTNODE_H
