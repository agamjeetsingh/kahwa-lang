//
// Created by Agamjeet Singh on 25/12/25.
//

#ifndef VARIABLESYMBOL_H
#define VARIABLESYMBOL_H
#include "Scope.h"
#include "Symbol.h"
#include "../parser/Modifier.h"

struct Type;

struct VariableSymbol : Symbol {
    VariableSymbol(std::string name, Scope* outerScope): Symbol(std::move(name), outerScope) {}

    ~VariableSymbol() noexcept override = default;

    bool isStatic = false;

    void setStatic(bool isStatic) {
        this->isStatic = isStatic;
    }

    Type* type = nullptr;

    void setType(Type* type) {
        this->type = type;
    }
};



#endif //VARIABLESYMBOL_H
