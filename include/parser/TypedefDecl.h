//
// Created by Agamjeet Singh on 03/12/25.
//

#ifndef TYPEDEFDECL_H
#define TYPEDEFDECL_H
#include <string>
#include <utility>

#include "TypeRef.h"


struct TypedefDecl : Decl {
    TypedefDecl(
        const std::string &name,
        const std::vector<ModifierNode>& modifiers,
        TypeRef* referredType,
        const SourceRange &typedefSourceRange,
        const SourceRange &nameSourceRange,
        const SourceRange &bodyRange):
    Decl(name, modifiers, nameSourceRange, bodyRange),
    typedefSourceRange(typedefSourceRange),
    referredType(referredType) {}

    const SourceRange typedefSourceRange;
    TypeRef* const referredType;

    bool operator==(const TypedefDecl &other) const {
        if (!Decl::operator==(other)) return false;
        
        if (referredType == nullptr && other.referredType == nullptr) {
            return true;
        }
        if (referredType == nullptr || other.referredType == nullptr) {
            return false;
        }
        return *referredType == *other.referredType;
    }
};

class TypedefDeclBuilder : public ASTBuilder{
public:
    explicit TypedefDeclBuilder(std::string name, TypeRef* referredType):
    name(std::move(name)), referredType(referredType) {}

    [[nodiscard]] TypedefDecl* build() const {
        return arena->make<TypedefDecl>(
            name,
            modifiers,
            referredType,
            typedefSourceRange.has_value() ? typedefSourceRange.value() : dummy_source,
            nameSourceRange.has_value() ? nameSourceRange.value() : dummy_source,
            bodyRange.has_value() ? bodyRange.value() : dummy_source);
    }

    TypedefDeclBuilder& with(Modifier modifier, const SourceRange &sourceRange = dummy_source) {
        modifiers.emplace_back(modifier, sourceRange);
        return *this;
    }

    TypedefDeclBuilder& with(const std::vector<Modifier>& modifiers) {
        return with(modifiers, std::vector(modifiers.size(), dummy_source));
    }

    TypedefDeclBuilder& with(std::vector<Modifier> modifiers, const std::vector<SourceRange>& sourceRanges) {
        assert(modifiers.size() == sourceRanges.size());
        for (int i = 0; i < modifiers.size(); i++) {
            this->modifiers.emplace_back(modifiers[i], sourceRanges[i]);
        }
        return *this;
    }

    TypedefDeclBuilder& withTypedefSourceRange(const SourceRange& typedefSourceRange) {
        this->typedefSourceRange.emplace(typedefSourceRange);
        return *this;
    }

    TypedefDeclBuilder& withNameSourceRange(const SourceRange& nameSourceRange) {
        this->nameSourceRange.emplace(nameSourceRange);
        return *this;
    }

    TypedefDeclBuilder& withBodyRange(const SourceRange& bodyRange) {
        this->bodyRange.emplace(bodyRange);
        return *this;
    }

private:
    std::string name;
    std::vector<ModifierNode> modifiers;
    TypeRef* referredType;
    std::optional<SourceRange> typedefSourceRange;
    std::optional<SourceRange> nameSourceRange;
    std::optional<SourceRange> bodyRange;
};


#endif //TYPEDEFDECL_H
