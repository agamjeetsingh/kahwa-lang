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


struct MethodDecl : Decl {
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

    TypeRef* const returnType;
    const std::vector<std::pair<TypeRef*, std::string>> parameters;
    Block* const block;

    const SourceRange returnTypeSourceRange;

    bool operator==(const MethodDecl &other) const {
        if (!Decl::operator==(other)) return false;
        
        if (returnType == nullptr && other.returnType == nullptr) {
            // Both null, compare other fields
        } else if (returnType == nullptr || other.returnType == nullptr) {
            return false;
        } else if (*returnType != *other.returnType) {
            return false;
        }
        
        if (parameters.size() != other.parameters.size()) return false;
        for (size_t i = 0; i < parameters.size(); ++i) {
            if (parameters[i].second != other.parameters[i].second) return false;
            
            auto &typeA = parameters[i].first;
            auto &typeB = other.parameters[i].first;
            if (typeA == nullptr && typeB == nullptr) continue;
            if (typeA == nullptr || typeB == nullptr) return false;
            if (*typeA != *typeB) return false;
        }
        
        if (block == nullptr && other.block == nullptr) {
            // Both null, continue
        } else if (block == nullptr || other.block == nullptr) {
            return false;
        } else if (*block != *other.block) {
            return false;
        }
        
        return returnTypeSourceRange == other.returnTypeSourceRange;
    }
};



#endif //METHODDECL_H
