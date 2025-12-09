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

    bool operator==(const ClassDecl &other) const {
        if (!Decl::operator==(other)) return false;
        
        if (superClasses.size() != other.superClasses.size() ||
            fields.size() != other.fields.size() ||
            methods.size() != other.methods.size() ||
            nestedClasses.size() != other.nestedClasses.size()) {
            return false;
        }
        
        for (size_t i = 0; i < superClasses.size(); ++i) {
            if (superClasses[i] == nullptr && other.superClasses[i] == nullptr) continue;
            if (superClasses[i] == nullptr || other.superClasses[i] == nullptr) return false;
            if (*superClasses[i] != *other.superClasses[i]) return false;
        }
        
        for (size_t i = 0; i < fields.size(); ++i) {
            if (fields[i] == nullptr && other.fields[i] == nullptr) continue;
            if (fields[i] == nullptr || other.fields[i] == nullptr) return false;
            if (*fields[i] != *other.fields[i]) return false;
        }
        
        for (size_t i = 0; i < methods.size(); ++i) {
            if (methods[i] == nullptr && other.methods[i] == nullptr) continue;
            if (methods[i] == nullptr || other.methods[i] == nullptr) return false;
            if (*methods[i] != *other.methods[i]) return false;
        }
        
        for (size_t i = 0; i < nestedClasses.size(); ++i) {
            if (nestedClasses[i] == nullptr && other.nestedClasses[i] == nullptr) continue;
            if (nestedClasses[i] == nullptr || other.nestedClasses[i] == nullptr) return false;
            if (*nestedClasses[i] != *other.nestedClasses[i]) return false;
        }
        
        return classSourceRange == other.classSourceRange;
    }
};



#endif //CLASS_H
