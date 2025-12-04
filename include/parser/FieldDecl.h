//
// Created by Agamjeet Singh on 29/11/25.
//

#ifndef FIELDDECL_H
#define FIELDDECL_H
#include <string>
#include <utility>

#include "Modifier.h"
#include "TypeRef.h"
#include "../source/SourceRange.h"


class FieldDecl {
public:
    FieldDecl(
    std::string name,
    const std::vector<Modifier> &modifiers,
    TypeRef* type,
    const SourceRange &typeSourceRange,
    const SourceRange &nameSourceRange,
    const SourceRange &bodyRange):
    name(std::move(name)),
    modifiers(modifiers),
    type(type),
    typeSourceRange(typeSourceRange),
    nameSourceRange(nameSourceRange),
    bodyRange(bodyRange) {}

private:
    std::string name;
    std::vector<Modifier> modifiers;
    TypeRef* type;
    // TODO - Initialiser expression (optional)

    SourceRange typeSourceRange;
    SourceRange nameSourceRange;
    SourceRange bodyRange;
};



#endif //FIELDDECL_H
