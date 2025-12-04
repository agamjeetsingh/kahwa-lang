//
// Created by Agamjeet Singh on 29/11/25.
//

#ifndef FIELDDECL_H
#define FIELDDECL_H
#include <string>
#include <utility>

#include "Decl.h"
#include "Modifier.h"
#include "TypeRef.h"
#include "../source/SourceRange.h"


struct FieldDecl : Decl {
    FieldDecl(
    std::string name,
    const std::vector<Modifier> &modifiers,
    TypeRef* type,
    const SourceRange &typeSourceRange,
    const SourceRange &nameSourceRange,
    const SourceRange &bodyRange):
    Decl(std::move(name), modifiers, nameSourceRange, bodyRange),
    type(type),
    typeSourceRange(typeSourceRange) {}

    TypeRef* const type;
    // TODO - Initialiser expression (optional)

    const SourceRange typeSourceRange;
};



#endif //FIELDDECL_H
