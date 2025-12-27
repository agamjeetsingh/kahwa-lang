//
// Created by Agamjeet Singh on 29/11/25.
//

#ifndef FIELDDECL_H
#define FIELDDECL_H
#include <string>
#include <utility>

#include "Decl.h"
#include "Modifier.h"
#include "TypeRef.h"
#include "../source/SourceRange.h"
#include "expr/Expr.h"
#include "expr/Stmt.h"

// Top-level variables, class fields and local variables are identical at the AST level (but not in symbols)
struct FieldDecl : Decl, Stmt {
    FieldDecl(
    std::string name,
    const std::vector<ModifierNode> &modifiers,
    TypeRef* typeRef,
    const SourceRange &typeSourceRange,
    const SourceRange &nameSourceRange,
    const SourceRange &bodyRange):
    Decl(std::move(name), modifiers, nameSourceRange, bodyRange),
    Stmt(bodyRange),
    typeRef(typeRef),
    typeSourceRange(typeSourceRange) {}

    TypeRef* const typeRef;
    Expr* initExpr = nullptr;

    const SourceRange typeSourceRange;

    bool operator==(const FieldDecl &other) const {
        if (!Decl::operator==(other)) return false;
        
        if (typeRef == nullptr && other.typeRef == nullptr) {
            return typeSourceRange == other.typeSourceRange;
        }
        if (typeRef == nullptr || other.typeRef == nullptr) {
            return false;
        }
        
        return *typeRef == *other.typeRef && typeSourceRange == other.typeSourceRange;
    }
};



#endif //FIELDDECL_H
