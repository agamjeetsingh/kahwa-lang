//
// Created by Agamjeet Singh on 03/12/25.
//

#ifndef TYPEDEFDECL_H
#define TYPEDEFDECL_H
#include <string>

#include "TypeRef.h"


struct TypedefDecl : Decl {
    TypedefDecl(
        const std::string &name,
        const std::vector<Modifier>& modifiers,
        TypeRef* referredType,
        const SourceRange &typedefSourceRange,
        const SourceRange &nameSourceRange,
        const SourceRange &bodyRange):
    Decl(name, modifiers, nameSourceRange, bodyRange),
    typedefSourceRange(typedefSourceRange),
    referredType(referredType) {}

    const SourceRange typedefSourceRange;
    TypeRef* const referredType;

    bool operator==(const TypedefDecl &other) const {
        if (!Decl::operator==(other)) return false;
        
        if (referredType == nullptr && other.referredType == nullptr) {
            return true;
        }
        if (referredType == nullptr || other.referredType == nullptr) {
            return false;
        }
        return *referredType == *other.referredType;
    }
};



#endif //TYPEDEFDECL_H
