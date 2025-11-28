//
// Created by Agamjeet Singh on 23/11/25.
//

#ifndef CLASS_H
#define CLASS_H

#include "Modifier.h"
#include "../diagnostics/DiagnosticEngine.h"

class ClassDecl {
public:
    void addModifier(Modifier modifier) {
        modifiers.push_back(modifier);
    }

    [[nodiscard]] const std::vector<Modifier>& getModifiers() const {
        return modifiers;
    }

private:
    std::vector<Modifier> modifiers;
};



#endif //CLASS_H
