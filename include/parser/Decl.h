//
// Created by Agamjeet Singh on 04/12/25.
//

#ifndef DECL_H
#define DECL_H
#include <utility>

#include "Modifier.h"
#include "../source/SourceRange.h"


struct Decl {
    Decl(std::string name,
    const std::vector<Modifier> &modifiers,
    const SourceRange &nameSourceRange,
    const SourceRange &bodyRange):
    name(std::move(name)),
    modifiers(modifiers),
    nameSourceRange(nameSourceRange),
    bodyRange(bodyRange) {}

    const std::string name;
    const std::vector<Modifier> modifiers;

    const SourceRange nameSourceRange;
    const SourceRange bodyRange;
};



#endif //DECL_H
