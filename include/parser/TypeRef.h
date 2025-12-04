//
// Created by Agamjeet Singh on 29/11/25.
//

#ifndef TYPEREF_H
#define TYPEREF_H
#include <vector>


class TypeRef {
public:
    explicit TypeRef(std::string identifier, const std::vector<TypeRef*> &args = {}): identifier(std::move(identifier)), args(args) {}

private:
    const std::string identifier;
    const std::vector<TypeRef*> args;
};



#endif //TYPEREF_H
