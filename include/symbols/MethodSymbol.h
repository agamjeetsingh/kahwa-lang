//
// Created by Agamjeet Singh on 19/12/25.
//

#ifndef METHODSYMBOL_H
#define METHODSYMBOL_H
#include "FunctionSymbol.h"


class MethodSymbol final : public FunctionSymbol {
public:
    explicit MethodSymbol(
        std::string name,
        Scope* outerScope): FunctionSymbol(std::move(name), outerScope) {}

    bool operator==(const MethodSymbol &) const {
        return true;
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

    bool isAbstract = false;
    bool isOpen = false;
};



#endif //METHODSYMBOL_H
