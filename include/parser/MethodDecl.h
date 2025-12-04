//
// Created by Agamjeet Singh on 29/11/25.
//

#ifndef METHODDECL_H
#define METHODDECL_H
#include <utility>

#include "Block.h"
#include "Decl.h"
#include "Modifier.h"
#include "TypeRef.h"
#include "../source/SourceRange.h"


class MethodDecl : public Decl {
public:
    MethodDecl(std::string name,
    const std::vector<Modifier> &modifiers,
    TypeRef* returnType,
    const std::vector<std::pair<TypeRef*, std::string>>& parameters,
    Block* block,
    const SourceRange &returnTypeSourceRange,
    const SourceRange &nameSourceRange,
    const SourceRange &bodyRange):
    Decl(std::move(name), modifiers, nameSourceRange, bodyRange),
    returnType(returnType),
    parameters(parameters),
    block(block),
    returnTypeSourceRange(returnTypeSourceRange) {}

private:
    TypeRef* const returnType;
    const std::vector<std::pair<TypeRef*, std::string>> parameters;
    Block* const block;

    const SourceRange returnTypeSourceRange;
};



#endif //METHODDECL_H
