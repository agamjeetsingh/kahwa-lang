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

    bool operator==(const TypeRef &other) const {
        if (identifier != other.identifier || args.size() != other.args.size()) {
            return false;
        }
        
        for (size_t i = 0; i < args.size(); ++i) {
            if (args[i] == nullptr && other.args[i] == nullptr) continue;
            if (args[i] == nullptr || other.args[i] == nullptr) return false;
            if (!(*args[i] == *other.args[i])) return false;
        }
        
        return true;
    }
};



#endif //TYPEREF_H
