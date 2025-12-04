//
// Created by Agamjeet Singh on 29/11/25.
//

#ifndef TYPEREF_H
#define TYPEREF_H
#include <vector>


struct TypeRef {
    explicit TypeRef(std::string identifier, const std::vector<TypeRef*> &args = {}): identifier(std::move(identifier)), args(args) {}

    const std::string identifier;
    const std::vector<TypeRef*> args;
};



#endif //TYPEREF_H
