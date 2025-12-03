//
// Created by Agamjeet Singh on 03/12/25.
//

#ifndef TYPEDEFDECL_H
#define TYPEDEFDECL_H
#include <string>

#include "TypeRef.h"


class TypedefDecl {
public:
    TypedefDecl(std::string typeDef, TypeRef* refferedType): typeDef(std::move(typeDef)), referredType(refferedType) {}

private:
    std::string typeDef;
    TypeRef* referredType;
};



#endif //TYPEDEFDECL_H
