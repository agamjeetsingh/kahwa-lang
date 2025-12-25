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

    bool isAbstract = false;
    bool isOpen = false;
};



#endif //METHODSYMBOL_H
