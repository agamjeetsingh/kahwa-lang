//
// Created by Agamjeet Singh on 18/12/25.
//

#ifndef FUNCTIONSYMBOL_H
#define FUNCTIONSYMBOL_H
#include <cassert>
#include <vector>

#include "Scope.h"
#include "Symbol.h"
#include "TypeParameterSymbol.h"
#include "../parser/Modifier.h"

enum class Modifier;
struct Block;
struct Type;

struct FunctionSymbol : Symbol {
    explicit FunctionSymbol(
        std::string name,
        Scope* outerScope): Symbol(std::move(name)) {
        scope.addOuterScope(outerScope);
    }

    explicit FunctionSymbol(
        std::string name,
        bool isAbstract,
        bool isOpen,
        Modifier visibility,
        const std::vector<TypeParameterSymbol*>& genericArguments,
        Type* returnType,
        Scope* outerScope,
        const std::vector<std::pair<Type*, std::string>>& parameters,
        Block* block):
    Symbol(std::move(name)),
    isAbstract(isAbstract),
    isOpen(isOpen),
    visibility(visibility),
    returnType(returnType),
    parameters(parameters),
    block(block),
    genericArguments(genericArguments) {
        // scope.defineAll(parameters); // TODO - Problem! Type* are not symbols! Or are they?
    }

    bool isAbstract = false;
    bool isOpen = false;
    Modifier visibility = Modifier::PUBLIC;

    Scope scope;

    Type* returnType = nullptr;
    std::vector<std::pair<Type*, std::string>> parameters;
    Block* block = nullptr;

    std::vector<TypeParameterSymbol*> genericArguments;

    void addGenericArguments(const std::vector<TypeParameterSymbol*>& genericArguments) {
        this->genericArguments.insert(this->genericArguments.begin(), genericArguments.begin(), genericArguments.end());
    }

    void addGenericArgument(TypeParameterSymbol* genericArgument) {
        genericArguments.push_back(genericArgument);
    }
};

class FunctionSymbolBuilder : public SymbolBuilder {
public:
    explicit FunctionSymbolBuilder(
        std::string name,
        Type* returnType,
        Block* block,
        Scope* outerScope):
    name(std::move(name)),
    returnType(returnType),
    block(block),
    outerScope(outerScope) {}

    FunctionSymbolBuilder& setModality(Modifier modifier) {
        assert(isModalityModifier(modifier));
        switch (modifier) {
            case Modifier::FINAL:
                isAbstract = false;
                isOpen = false;
                break;
            case Modifier::OPEN:
                isAbstract = false;
                isOpen = true;
                break;
            case Modifier::ABSTRACT:
                isAbstract = true;
                isOpen = true;
                break;
            default:
                throw std::runtime_error("Unreachable");
        }
        return *this;
    }

    FunctionSymbolBuilder& setVisibility(Modifier visibility) {
        assert(isVisibilityModifier(visibility));
        this->visibility = visibility;
        return *this;
    }

    FunctionSymbolBuilder& with(TypeParameterSymbol* genericArgument) {
        genericArguments.push_back(genericArgument);
        return *this;
    }

    FunctionSymbolBuilder& with(const std::string& genericArgumentName, const Variance variance = Variance::INVARIANT) {
        genericArguments.push_back(arena->make<TypeParameterSymbol>(genericArgumentName, variance));
        return *this;
    }

    FunctionSymbolBuilder& withNumOfGenericArgs(std::size_t numberOfGenericArguments) {
        std::vector<std::string> genericArgumentNames(numberOfGenericArguments);
        std::ranges::generate(genericArgumentNames, [i = 1]() mutable { return "__dummyTypeParameter" + std::to_string(i++); });

        return with(genericArgumentNames);
    }

    FunctionSymbolBuilder& with(const std::vector<std::string>& genericArgumentNames) {
        return with(genericArgumentNames, std::vector(genericArgumentNames.size(), Variance::INVARIANT));
    }

    FunctionSymbolBuilder& with(const std::vector<std::string>& genericArgumentNames, const std::vector<Variance>& variances) {
        assert(genericArgumentNames.size() == variances.size());

        for (int i = 0; i < genericArgumentNames.size(); i++) {
            genericArguments.push_back(arena->make<TypeParameterSymbol>(genericArgumentNames[i], variances[i]));
        }
        return *this;
    }

    FunctionSymbolBuilder& with(std::vector<TypeParameterSymbol*> genericArguments) {
        this->genericArguments.insert(this->genericArguments.end(), genericArguments.begin(), genericArguments.end());
        return *this;
    }

private:
    std::string name;
    bool isAbstract = false;
    bool isOpen = false;
    Modifier visibility = Modifier::PUBLIC;
    std::vector<TypeParameterSymbol*> genericArguments;
    Type* returnType;
    std::vector<std::pair<Type*, std::string>> parameters;
    Block* block;
    Scope* outerScope;
};



#endif //FUNCTIONSYMBOL_H
