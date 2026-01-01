//
// Created by Agamjeet Singh on 20/12/25.
//

#ifndef MODIFIERNODE_H
#define MODIFIERNODE_H
#include "ASTNode.h"
#include "Modifier.h"


struct ModifierNode : ASTNode {
    ModifierNode(Modifier modifier, const SourceRange &bodyRange): ASTNode(bodyRange), modifier(modifier) {}

    Modifier modifier;

    void accept(ASTVisitor &v) override {
        v.visit(this);
    }

    void visitChildren(ASTVisitor &v) override {}

    bool operator==(const ModifierNode &other) const {
        return modifier == other.modifier && bodyRange == other.bodyRange;
    }
};



#endif //MODIFIERNODE_H
