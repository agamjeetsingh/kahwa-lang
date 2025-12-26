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

    void setOverride(bool isAnOverride) {
        this->isAnOverride = isAnOverride;
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
    bool isAnOverride = false;
};



#endif //FIELDSYMBOL_H
