//
// Created by Agamjeet Singh on 22/12/25.
//

#ifndef TRANSLATIONUNIT_H
#define TRANSLATIONUNIT_H
#include <vector>

#include "Scope.h"


struct TranslationUnit {
    std::vector<ClassSymbol*> classes;

    Scope scope;
};



#endif //TRANSLATIONUNIT_H
