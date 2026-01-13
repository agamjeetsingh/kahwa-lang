//
// Created by Agamjeet Singh on 04/12/25.
//

#ifndef DECL_H
#define DECL_H
#include <utility>

#include "Modifier.h"
#include "ModifierNode.h"
#include "../source/SourceRange.h"
#include "ASTNode.h"


struct Decl : ASTNode {
    Decl(std::string name,
    const std::vector<ModifierNode*> &modifiers,
    const SourceRange &nameSourceRange,
    const SourceRange &bodyRange):
    ASTNode(bodyRange),
    name(std::move(name)),
    modifiers(modifiers),
    nameSourceRange(nameSourceRange) {}

    const std::string name;
    const std::vector<ModifierNode*> modifiers;

    const SourceRange nameSourceRange;

    bool operator==(const Decl &other) const {
        return name == other.name &&
               modifiers == other.modifiers &&
               nameSourceRange == other.nameSourceRange &&
               bodyRange == other.bodyRange;
    }
};



#endif //DECL_H
