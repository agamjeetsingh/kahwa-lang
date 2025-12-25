//
// Created by Agamjeet Singh on 20/12/25.
//

#ifndef TYPESYMBOL_H
#define TYPESYMBOL_H
#include "Symbol.h"


class TypeSymbol : public Symbol {
public:
    explicit TypeSymbol(std::string name, Scope* outerScope): Symbol(std::move(name), outerScope) {}
};



#endif //TYPESYMBOL_H
