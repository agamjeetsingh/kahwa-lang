//
// Created by Agamjeet Singh on 20/12/25.
//

#ifndef MODIFIERNODE_H
#define MODIFIERNODE_H
#include "Modifier.h"


struct ModifierNode {
    const Modifier modifier;
    const SourceRange sourceRange;

    bool operator==(const ModifierNode &other) const {
        return modifier == other.modifier && sourceRange == other.sourceRange;
    }
};



#endif //MODIFIERNODE_H
