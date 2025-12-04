//
// Created by Agamjeet Singh on 29/11/25.
//

#ifndef METHODDECL_H
#define METHODDECL_H
#include <utility>

#include "Block.h"
#include "Modifier.h"
#include "TypeRef.h"
#include "../source/SourceRange.h"


class MethodDecl {
public:
    MethodDecl(std::string name,
    const std::vector<Modifier> &modifiers,
    TypeRef* returnType,
    const std::vector<std::pair<TypeRef*, std::string>>& parameters,
    Block* block,
    const SourceRange &returnTypeSourceRange,
    const SourceRange &nameSourceRange,
    const SourceRange &bodyRange):
    name(std::move(name)),
    modifiers(modifiers),
    returnType(returnType),
    parameters(parameters),
    block(block),
    returnTypeSourceRange(returnTypeSourceRange),
    nameSourceRange(nameSourceRange),
    bodyRange(bodyRange) {}

private:
    std::string name;
    std::vector<Modifier> modifiers;
    TypeRef* returnType;
    std::vector<std::pair<TypeRef*, std::string>> parameters;
    Block* block;

    SourceRange returnTypeSourceRange;
    SourceRange nameSourceRange;
    SourceRange bodyRange;
};



#endif //METHODDECL_H
