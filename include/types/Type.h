//
// Created by Agamjeet Singh on 20/12/25.
//

#ifndef TYPE_H
#define TYPE_H
#include <utility>

#include "Variance.h"
#include "../symbols/ClassSymbol.h"
#include "../symbols/TypeParameterSymbol.h"

class TypeBuilder;

struct Type {
    explicit Type(TypeSymbol* typeSymbol, const std::vector<Type*>& genericArguments = {}): typeSymbol(typeSymbol), genericArguments(genericArguments) {
        if (auto ptr = dynamic_cast<ClassSymbol*>(typeSymbol)) {
            assert(ptr->genericArguments.size() == genericArguments.size());
        } else {
            assert(genericArguments.empty());
        }

        toString = typeSymbol->name;
        if (!genericArguments.empty()) {
            toString += "<";

            for (int i = 0; i < genericArguments.size(); i++) {
                toString += genericArguments[i]->toString;

                if (i != genericArguments.size() - 1) {
                    toString += ", ";
                }
            }

            toString += ">";
        }
    }

    TypeSymbol* typeSymbol;

    const std::vector<Type*> genericArguments;

    bool operator==(const Type &other) const {
        return this->isSubtypeOf(&other) && other.isSubtypeOf(this);
    }

    bool isSubtypeOf(const Type* other) const {
        // If any type is not concrete, then both must be equal
        if (!dynamic_cast<ClassSymbol*>(typeSymbol) || !dynamic_cast<ClassSymbol*>(other->typeSymbol)) {
            auto ptr1 = dynamic_cast<TypeParameterSymbol*>(typeSymbol);
            auto ptr2 = dynamic_cast<TypeParameterSymbol*>(other->typeSymbol);
            return ptr1 && ptr2 && *ptr1 == *ptr2;
        }
        auto classSymbol = dynamic_cast<ClassSymbol*>(this->typeSymbol);
        auto otherClassSymbol = dynamic_cast<ClassSymbol*>(other->typeSymbol);
        if (classSymbol == otherClassSymbol) {
            // Check that all variance rules are being satisfied
            for (int i = 0; i < genericArguments.size(); i++) {
                auto type1 = genericArguments[i];
                auto type2 = other->genericArguments[i];
                auto variance = classSymbol->genericArguments[i]->variance;
                if (variance == Variance::INVARIANT && *type1 != *type2) {
                    return false;
                }
                if (variance == Variance::CONTRAVARIANT && !type2->isSubtypeOf(type1)) {
                    return false;
                }
                if (variance == Variance::COVARIANT && !type1->isSubtypeOf(type2)) return false;
            }
            return true;
        } else {
            std::unordered_map<TypeParameterSymbol*, int> map;
            for (int i = 0; i < classSymbol->genericArguments.size(); i++) {
                map[classSymbol->genericArguments[i]] = i;
            }

            for (auto superClass: classSymbol->superClasses) {
                auto convertedType = substitute(classSymbol->genericArguments, superClass, this);
                // Should have: this <: converted
                assert(convertedType->typeSymbol == superClass->typeSymbol);
                if (convertedType->isSubtypeOf(other)) {
                    return true;
                }
            }

            return false;
        }
    }

    static Type* substitute(const std::vector<TypeParameterSymbol*>& subTypeBluePrint, const Type* superType, const Type* actualSubType);

private:
    std::string toString;
};

class TypeBuilder : SymbolBuilder {
public:
    explicit TypeBuilder(TypeSymbol* typeSymbol): typeSymbol(typeSymbol) {}

    explicit TypeBuilder(std::string typeParameterName, Variance variance = Variance::INVARIANT): typeSymbol(TypeParameterSymbolBuilder(std::move(typeParameterName)).with(variance).build()) {}

    Type* build() {
        return arena->make<Type>(typeSymbol, genericArguments);
    }

    TypeBuilder& with(const std::vector<Type*>& genericArguments) {
        this->genericArguments.insert(this->genericArguments.end(), genericArguments.begin(), genericArguments.end());
        return *this;
    }

    TypeBuilder& with(Type* genericArgument) {
        genericArguments.push_back(genericArgument);
        return *this;
    }

private:
    TypeSymbol* typeSymbol;
    std::vector<Type*> genericArguments;
};



#endif //TYPE_H
