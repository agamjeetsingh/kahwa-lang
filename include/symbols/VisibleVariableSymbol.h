//
// Created by Agamjeet Singh on 25/12/25.
//

#ifndef VISIBLEVARIABLESYMBOL_H
#define VISIBLEVARIABLESYMBOL_H
#include "VariableSymbol.h"
#include "../parser/Modifier.h"


struct VisibleVariableSymbol : VariableSymbol {
    VisibleVariableSymbol(std::string name, Scope* outerScope): VariableSymbol(std::move(name), outerScope) {}

    ~VisibleVariableSymbol() noexcept override = default;

    Modifier visibility = Modifier::PUBLIC;

    void setVisibility(Modifier visibility) {
        this->visibility = visibility;
    }
};



#endif //VISIBLEVARIABLESYMBOL_H
