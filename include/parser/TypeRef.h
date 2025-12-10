//
// Created by Agamjeet Singh on 29/11/25.
//

#ifndef TYPEREF_H
#define TYPEREF_H
#include <vector>
#include "ASTBuilder.h"

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

class TypeRefBuilder : public ASTBuilder{
public:
    explicit TypeRefBuilder(const std::string& name): name(name) {}

    [[nodiscard]] TypeRef* build() const {
        return arena->make<TypeRef>(name, typeRefs);
    }

    TypeRefBuilder& with(TypeRef* typeRef) {
        typeRefs.push_back(typeRef);
        return *this;
    }

    TypeRefBuilder& with(std::vector<TypeRef*> typeRefs) {
        this->typeRefs.insert(this->typeRefs.end(), typeRefs.begin(), typeRefs.end());
        return *this;
    }

private:
    std::string name;
    std::vector<TypeRef*> typeRefs;
};



#endif //TYPEREF_H
