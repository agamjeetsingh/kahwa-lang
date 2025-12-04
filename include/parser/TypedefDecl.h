//
// Created by Agamjeet Singh on 03/12/25.
//

#ifndef TYPEDEFDECL_H
#define TYPEDEFDECL_H
#include <string>

#include "TypeRef.h"


struct TypedefDecl {
    TypedefDecl(std::string typeDef, TypeRef* refferedType): typeDef(std::move(typeDef)), referredType(refferedType) {}

    const std::string typeDef;
    TypeRef* const referredType;
};



#endif //TYPEDEFDECL_H
