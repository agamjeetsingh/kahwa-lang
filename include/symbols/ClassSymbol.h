//
// Created by Agamjeet Singh on 18/12/25.
//

#ifndef CLASSSYMBOL_H
#define CLASSSYMBOL_H
#include <vector>

#include "MethodSymbol.h"
#include "TypeParameterSymbol.h"
#include "../parser/Modifier.h"
#include "SymbolBuilder.h"


struct ClassSymbol {
    explicit ClassSymbol(
        std::string name,
        bool isAbstract,
        bool isOpen,
        Modifier visibility,
        const std::vector<TypeParameterSymbol*>& genericArguments,
        const std::vector<ClassSymbol*>& superClasses,
        const std::vector<MethodSymbol*>& methods,
        const std::vector<ClassSymbol*>& nestedClasses):
    name(std::move(name)),
    isAbstract(isAbstract),
    isOpen(isOpen),
    visibility(visibility),
    genericArguments(genericArguments),
    superClasses(superClasses),
    methods(methods),
    nestedClasses(nestedClasses) {}

    std::string name;
    bool isAbstract;
    bool isOpen;
    Modifier visibility;

    const std::vector<TypeParameterSymbol*> genericArguments;
    const std::vector<ClassSymbol*> superClasses;
    const std::vector<MethodSymbol*> methods;
    const std::vector<ClassSymbol*> nestedClasses;

    static constexpr auto DEFAULT_VISIBILITY = Modifier::PUBLIC;
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

    ClassSymbolBuilder& with(std::vector<TypeParameterSymbol*> genericArguments) {
        this->genericArguments.insert(this->genericArguments.end(), genericArguments.begin(), genericArguments.end());
        return *this;
    }

    ClassSymbolBuilder& withSuperClass(ClassSymbol* superClass) {
        superClasses.push_back(superClass);
        return *this;
    }

    ClassSymbolBuilder& withSuperClasses(std::vector<ClassSymbol*> superClasses) {
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
    Modifier visibility = ClassSymbol::DEFAULT_VISIBILITY;

    std::vector<TypeParameterSymbol*> genericArguments;
    std::vector<ClassSymbol*> superClasses;
    std::vector<MethodSymbol*> methods;
    std::vector<ClassSymbol*> nestedClasses;
};


#endif //CLASSSYMBOL_H
