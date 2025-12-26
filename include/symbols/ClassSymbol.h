//
// Created by Agamjeet Singh on 18/12/25.
//

#ifndef CLASSSYMBOL_H
#define CLASSSYMBOL_H
#include <vector>

#include "FieldSymbol.h"
#include "MethodSymbol.h"
#include "Scope.h"
#include "../parser/Modifier.h"
#include "SymbolBuilder.h"
#include "TypeSymbol.h"
#include "TypeParameterSymbol.h"
#include "../types/Variance.h"

struct Type;

struct ClassSymbol final : TypeSymbol {
    explicit ClassSymbol(
        std::string name,
        bool isAbstract,
        bool isOpen,
        Modifier visibility,
        const std::vector<TypeParameterSymbol*>& genericArguments,
        const std::vector<Type*>& superClasses,
        const std::vector<MethodSymbol*>& methods,
        const std::vector<ClassSymbol*>& nestedClasses,
        Scope* outerScope = nullptr): // TODO
    TypeSymbol(std::move(name), outerScope),
    isAbstract(isAbstract),
    isOpen(isOpen),
    visibility(visibility),
    genericArguments(genericArguments),
    superClasses(superClasses),
    methods(methods),
    nestedClasses(nestedClasses) {}

    explicit ClassSymbol(
        std::string name,
        Scope* outerScope):
    TypeSymbol(std::move(name), outerScope) {}

    void addGenericArguments(const std::vector<TypeParameterSymbol*>& genericArguments) {
        this->genericArguments.insert(this->genericArguments.begin(), genericArguments.begin(), genericArguments.end());
    }

    void addGenericArgument(TypeParameterSymbol* genericArgument) {
        genericArguments.push_back(genericArgument);
    }

    void addSuperClasses(const std::vector<Type*>& superClasses) {
        this->superClasses.insert(this->superClasses.begin(), superClasses.begin(), superClasses.end());
    }

    void addSuperClass(Type* superClass) {
        superClasses.push_back(superClass);
    }

    void setModality(Modifier modifier) {
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
    }

    void setVisibility(Modifier visibility) {
        assert(isVisibilityModifier(visibility));
        this->visibility = visibility;
    }

    void addMethods(const std::vector<MethodSymbol*>& methods) {
        this->methods.insert(this->methods.begin(), methods.begin(), methods.end());
    }

    void addMethod(MethodSymbol* method) {
        methods.push_back(method);
    }

    void addFields(const std::vector<FieldSymbol*>& fields) {
        this->fields.insert(this->fields.begin(), fields.begin(), fields.end());
    }

    void addField(FieldSymbol* field) {
        fields.push_back(field);
    }

    void addNestedClasses(const std::vector<ClassSymbol*>& nestedClasses) {
        this->nestedClasses.insert(this->nestedClasses.begin(), nestedClasses.begin(), nestedClasses.end());
    }

    void addNestedClass(ClassSymbol* nestedClass) {
        nestedClasses.push_back(nestedClass);
    }

    bool isAbstract = false;
    bool isOpen = false;
    Modifier visibility = Modifier::PUBLIC;

    std::vector<TypeParameterSymbol*> genericArguments;
    std::vector<Type*> superClasses;
    std::vector<MethodSymbol*> methods;
    std::vector<FieldSymbol*> fields;
    std::vector<ClassSymbol*> nestedClasses;

    bool operator==(const ClassSymbol &other) const {
        return this == &other;
    }
};


class ClassSymbolBuilder : public SymbolBuilder {
public:
    explicit ClassSymbolBuilder(std::string name): name(std::move(name)) {}

    ClassSymbol* build() {
        return arena->make<ClassSymbol>(name, isAbstract, isOpen, visibility, genericArguments, superClasses, methods, nestedClasses);
    }

    ClassSymbolBuilder& setModality(Modifier modifier) {
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

    ClassSymbolBuilder& setVisibility(Modifier visibility) {
        assert(isVisibilityModifier(visibility));
        this->visibility = visibility;
        return *this;
    }

    ClassSymbolBuilder& with(Modifier modifier) {
        if (isVisibilityModifier(modifier)) {
            setVisibility(modifier);
        } else if (isModalityModifier(modifier)) {
            setModality(modifier);
        }
        return *this;
    }

    ClassSymbolBuilder& with(TypeParameterSymbol* genericArgument) {
        genericArguments.push_back(genericArgument);
        return *this;
    }

    ClassSymbolBuilder& with(const std::string& genericArgumentName, const Variance variance = Variance::INVARIANT) {
        genericArguments.push_back(arena->make<TypeParameterSymbol>(genericArgumentName, nullptr, variance)); // TODO
        return *this;
    }

    ClassSymbolBuilder& withNumOfGenericArgs(std::size_t numberOfGenericArguments) {
        std::vector<std::string> genericArgumentNames(numberOfGenericArguments);
        std::ranges::generate(genericArgumentNames, [i = 1]() mutable { return "__dummyTypeParameter" + std::to_string(i++); });

        return with(genericArgumentNames);
    }

    ClassSymbolBuilder& with(const std::vector<std::string>& genericArgumentNames) {
        return with(genericArgumentNames, std::vector(genericArgumentNames.size(), Variance::INVARIANT));
    }

    ClassSymbolBuilder& with(const std::vector<std::string>& genericArgumentNames, const std::vector<Variance>& variances) {
        assert(genericArgumentNames.size() == variances.size());

        for (int i = 0; i < genericArgumentNames.size(); i++) {
            genericArguments.push_back(arena->make<TypeParameterSymbol>(genericArgumentNames[i], nullptr, variances[i])); // TODO
        }
        return *this;
    }

    ClassSymbolBuilder& with(std::vector<TypeParameterSymbol*> genericArguments) {
        this->genericArguments.insert(this->genericArguments.end(), genericArguments.begin(), genericArguments.end());
        return *this;
    }

    ClassSymbolBuilder& withSuperClass(Type* superClass) {
        superClasses.push_back(superClass);
        return *this;
    }

    ClassSymbolBuilder& withSuperClasses(std::vector<Type*> superClasses) {
        this->superClasses.insert(this->superClasses.end(), superClasses.begin(), superClasses.end());
        return *this;
    }

    ClassSymbolBuilder& with(MethodSymbol* method) {
        methods.push_back(method);
        return *this;
    }

    ClassSymbolBuilder& with(std::vector<MethodSymbol*> methods) {
        this->methods.insert(this->methods.end(), methods.begin(), methods.end());
        return *this;
    }

    ClassSymbolBuilder& withNestedClass(ClassSymbol* nestedClass) {
        nestedClasses.push_back(nestedClass);
        return *this;
    }

    ClassSymbolBuilder& withNestedClasses(std::vector<ClassSymbol*> nestedClasses) {
        this->nestedClasses.insert(this->nestedClasses.end(), nestedClasses.begin(), nestedClasses.end());
        return *this;
    }

private:
    std::string name;
    bool isAbstract = false;
    bool isOpen = false;
    Modifier visibility = Modifier::PUBLIC;

    std::vector<TypeParameterSymbol*> genericArguments;
    std::vector<Type*> superClasses;
    std::vector<MethodSymbol*> methods;
    std::vector<ClassSymbol*> nestedClasses;
};


#endif //CLASSSYMBOL_H
