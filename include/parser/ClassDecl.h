//
// Created by Agamjeet Singh on 23/11/25.
//

#ifndef CLASS_H
#define CLASS_H

#include <utility>

#include "Decl.h"
#include "FieldDecl.h"
#include "MethodDecl.h"
#include "Modifier.h"
#include "TypeRef.h"
#include "../tokeniser/Token.h"

struct ClassDecl : Decl {
    ClassDecl(std::string name,
        const SourceRange &classSourceRange,
        const SourceRange &nameSourceRange,
        const SourceRange &bodyRange,
        const std::vector<Modifier> &modifiers = {},
        const std::vector<TypeRef*>& superClasses = {},
        const std::vector<FieldDecl*> &fields = {},
        const std::vector<MethodDecl*> &methods = {},
        const std::vector<ClassDecl*> &nestedClasses = {}
        ):
    Decl(std::move(name), modifiers, nameSourceRange, bodyRange),
    superClasses(superClasses),
    fields(fields),
    methods(methods),
    nestedClasses(nestedClasses),
    classSourceRange(classSourceRange) {}

    const std::vector<TypeRef*> superClasses;
    const std::vector<FieldDecl*> fields;
    const std::vector<MethodDecl*> methods;
    const std::vector<ClassDecl*> nestedClasses;

    const SourceRange classSourceRange;
};



#endif //CLASS_H
