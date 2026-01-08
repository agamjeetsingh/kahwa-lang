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
struct FieldDecl : Decl {
    FieldDecl(
    std::string name,
    const std::vector<ModifierNode*> &modifiers,
    TypeRef* typeRef,
    const SourceRange &typeSourceRange,
    const SourceRange &nameSourceRange,
    const SourceRange &bodyRange,
    Expr* initExpr):
    Decl(std::move(name), modifiers, nameSourceRange, bodyRange),
    typeRef(typeRef),
    initExpr(initExpr),
    typeSourceRange(typeSourceRange) {}

    TypeRef* const typeRef;
    Expr* initExpr = nullptr;

    const SourceRange typeSourceRange;

    void accept(ASTVisitor &v) override {
        v.visit(this);
    }

    void visitChildren(ASTVisitor &v) override {
        typeRef->accept(v);
        if (initExpr) initExpr->accept(v);
        std::ranges::for_each(modifiers, [&v](ModifierNode* modifierNode) { modifierNode->accept(v); });
    }

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

class FieldDeclBuilder : public ASTBuilder {
public:
    FieldDeclBuilder(std::string name, TypeRef* typeRef): name(std::move(name)), typeRef(typeRef) {}

    FieldDecl* build() {
        return arena->make<FieldDecl>(
            name,
            modifiers,
            typeRef,
            dummy_source,
            dummy_source,
            dummy_source,
            initExpr);
    }

    FieldDeclBuilder& with(Modifier modifier, const SourceRange &sourceRange = dummy_source) {
        modifiers.push_back(arena->make<ModifierNode>(modifier, sourceRange));
        return *this;
    }

    FieldDeclBuilder& with(const std::vector<Modifier>& modifiers) {
        return with(modifiers, std::vector(modifiers.size(), dummy_source));
    }

    FieldDeclBuilder& with(const std::vector<Modifier>& modifiers, const std::vector<SourceRange>& sourceRanges) {
        assert(modifiers.size() == sourceRanges.size());
        for (int i = 0; i < modifiers.size(); i++) {
            this->modifiers.push_back(arena->make<ModifierNode>(modifiers[i], sourceRanges[i]));
        }
        return *this;
    }

    FieldDeclBuilder& with(const std::vector<ModifierNode*>& modifierNodes) {
        this->modifiers.insert(this->modifiers.end(), modifierNodes.begin(), modifierNodes.end());
        return *this;
    }

    FieldDeclBuilder& with(Expr* initExpr) {
        this->initExpr = initExpr;
        return *this;
    }

private:
    std::vector<ModifierNode*> modifiers;
    std::string name;
    TypeRef* typeRef;
    Expr* initExpr = nullptr;
};


#endif //FIELDDECL_H
