//
// Created by Agamjeet Singh on 21/12/25.
//

#ifndef SYMBOL_H
#define SYMBOL_H
#include <string>

#include "Scope.h"


struct Symbol {
    explicit Symbol(std::string name, Scope* outerScope): name(std::move(name)) {
        scope.addOuterScope(outerScope);
    }

    const std::string name;
    Scope scope;

    virtual ~Symbol() noexcept = default;
};



#endif //SYMBOL_H
