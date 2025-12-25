//
// Created by Agamjeet Singh on 22/12/25.
//

#ifndef TRANSLATIONUNIT_H
#define TRANSLATIONUNIT_H
#include <vector>

#include "ClassSymbol.h"
#include "FunctionSymbol.h"
#include "Scope.h"
#include "VisibleVariableSymbol.h"


struct TranslationUnit {
    std::vector<ClassSymbol*> classes;
    std::vector<FunctionSymbol*> functions;
    std::vector<VisibleVariableSymbol*> variables;

    Scope scope;

    void addClasses(const std::vector<ClassSymbol*>& classes) {
        this->classes.insert(this->classes.begin(), classes.begin(), classes.end());
    }

    void addClass(ClassSymbol* clazz) {
        this->classes.push_back(clazz);
    }

    void addFunctions(const std::vector<FunctionSymbol*>& functions) {
        this->functions.insert(this->functions.begin(), functions.begin(), functions.end());
    }

    void addFunction(FunctionSymbol* function) {
        functions.push_back(function);
    }

    void addVariables(const std::vector<VisibleVariableSymbol*>& variables) {
        this->variables.insert(this->variables.begin(), variables.begin(), variables.end());
    }

    void addVariable(VisibleVariableSymbol* variable) {
        variables.push_back(variable);
    }
};



#endif //TRANSLATIONUNIT_H
