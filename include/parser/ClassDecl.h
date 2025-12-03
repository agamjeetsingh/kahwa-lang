//
// Created by Agamjeet Singh on 23/11/25.
//

#ifndef CLASS_H
#define CLASS_H

#include <utility>

#include "FieldDecl.h"
#include "MethodDecl.h"
#include "Modifier.h"
#include "../tokeniser/Token.h"

class ClassDecl {
public:
    ClassDecl(std::string name,
        const SourceRange &classSourceRange,
        const SourceRange &nameSourceRange,
        const SourceRange &bodyRange,
        const std::vector<Modifier> &modifiers = {},
        const std::vector<FieldDecl*> &fields = {},
        const std::vector<MethodDecl*> &methods = {},
        const std::vector<ClassDecl*> &nestedClasses = {}
        ):
    name(std::move(name)),
    modifiers(modifiers),
    fields(fields),
    methods(methods),
    nestedClasses(nestedClasses),
    classSourceRange(classSourceRange),
    nameSourceRange(nameSourceRange),
    bodyRange(bodyRange) {}

private:
    std::string name;
    std::vector<Modifier> modifiers;
    std::vector<FieldDecl*> fields;
    std::vector<MethodDecl*> methods;
    std::vector<ClassDecl*> nestedClasses;

    SourceRange classSourceRange;
    SourceRange nameSourceRange;
    SourceRange bodyRange;
};



#endif //CLASS_H
