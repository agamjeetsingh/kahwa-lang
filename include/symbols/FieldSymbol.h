//
// Created by Agamjeet Singh on 25/12/25.
//

#ifndef FIELDSYMBOL_H
#define FIELDSYMBOL_H
#include <string>

#include "Scope.h"
#include "VisibleVariableSymbol.h"


struct FieldSymbol : VisibleVariableSymbol {
    FieldSymbol(std::string name, Scope* outerScope): VisibleVariableSymbol(std::move(name), outerScope) {}

    bool isAbstract = false;
    bool isOpen = false;
};



#endif //FIELDSYMBOL_H
