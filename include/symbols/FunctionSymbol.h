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
#include "VariableSymbol.h"
#include "../parser/Modifier.h"

enum class Modifier;
struct Block;
struct Type;

struct FunctionSymbol : Symbol {
    explicit FunctionSymbol(
        std::string name,
        Scope* outerScope): Symbol(std::move(name), outerScope) {}

    explicit FunctionSymbol(
        std::string name,
        Modifier visibility,
        const std::vector<TypeParameterSymbol*>& genericArguments,
        Type* returnType,
        Scope* outerScope,
        const std::vector<VariableSymbol*>& parameters,
        Block* block):
    Symbol(std::move(name), outerScope),
    visibility(visibility),
    returnType(returnType),
    parameters(parameters),
    block(block),
    genericArguments(genericArguments) {}

    Modifier visibility = Modifier::PUBLIC;

    Type* returnType = nullptr;
    std::vector<VariableSymbol*> parameters;
    Block* block = nullptr;

    std::vector<TypeParameterSymbol*> genericArguments;

    void addGenericArguments(const std::vector<TypeParameterSymbol*>& genericArguments) {
        this->genericArguments.insert(this->genericArguments.end(), genericArguments.begin(), genericArguments.end());
    }

    void addGenericArgument(TypeParameterSymbol* genericArgument) {
        genericArguments.push_back(genericArgument);
    }

    void addParameters(const std::vector<VariableSymbol*>& parameters) {
        this->parameters.insert(this->parameters.end(), parameters.begin(), parameters.end());
    }

    void addParameter(VariableSymbol* parameter) {
        parameters.push_back(parameter);
    }

    void setVisibility(Modifier visibility) {
        assert(isVisibilityModifier(visibility));
        this->visibility = visibility;
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
        genericArguments.push_back(arena->make<TypeParameterSymbol>(genericArgumentName, nullptr, variance)); // TODO
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
            genericArguments.push_back(arena->make<TypeParameterSymbol>(genericArgumentNames[i], nullptr, variances[i])); // TODO
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
