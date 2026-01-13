//
// Created by Agamjeet Singh on 29/11/25.
//

#ifndef METHODDECL_H
#define METHODDECL_H
#include <utility>

#include "Block.h"
#include "Decl.h"
#include "Modifier.h"
#include "TypeRef.h"
#include "../source/SourceRange.h"
#include "FieldDecl.h"
#include "TypeParameterDecl.h"

// Functions and Methods are identical at the AST level (but not in symbols)
struct MethodDecl : Decl {
    MethodDecl(std::string name,
    const std::vector<ModifierNode*> &modifiers,
    TypeRef* returnType,
    const std::vector<FieldDecl*>& parameters,
    Block* block,
    const std::vector<TypeParameterDecl*> &typeParameters,
    const SourceRange &returnTypeSourceRange,
    const SourceRange &nameSourceRange,
    const SourceRange &bodyRange):
    Decl(std::move(name), modifiers, nameSourceRange, bodyRange),
    returnType(returnType),
    parameters(parameters),
    block(block),
    typeParameters(typeParameters),
    returnTypeSourceRange(returnTypeSourceRange) {}

    TypeRef* returnType;
    const std::vector<FieldDecl*> parameters;
    Block* block;

    const std::vector<TypeParameterDecl*> typeParameters;

    const SourceRange returnTypeSourceRange;

    void visitChildren(ASTVisitor &v) override {
        std::ranges::for_each(modifiers, [&v](ModifierNode* node) { node->accept(v); });
        returnType->accept(v);
        // TODO - I think parameters should be stored as some kind of parameter decl
        block->accept(v);
        std::ranges::for_each(typeParameters, [&v](TypeParameterDecl* node) { node->accept(v); });
        std::ranges::for_each(parameters, [&v](FieldDecl* node) { node->accept(v); });
    }

    void accept(ASTVisitor &v) override {
        v.visit(this);
    }

    bool operator==(const MethodDecl &other) const {
        if (!Decl::operator==(other)) return false;
        
        if (returnType == nullptr && other.returnType == nullptr) {
            // Both null, compare other fields
        } else if (returnType == nullptr || other.returnType == nullptr) {
            return false;
        } else if (*returnType != *other.returnType) {
            return false;
        }

        if (parameters.size() != other.parameters.size()) return false;
        for (size_t i = 0; i < parameters.size(); ++i) {
            if (*parameters[i] != *other.parameters[i]) return false;
        }
        
        if (block == nullptr && other.block == nullptr) {
            // Both null, continue
        } else if (block == nullptr || other.block == nullptr) {
            return false;
        } else if (*block != *other.block) {
            return false;
        }
        
        return returnTypeSourceRange == other.returnTypeSourceRange;
    }
};

class MethodDeclBuilder : public ASTBuilder {
public:
    MethodDeclBuilder(std::string  name, TypeRef* returnType, Block* block)
        : name(std::move(name)), returnType(returnType), block(block) {}

    [[nodiscard]] MethodDecl* build() {
        return arena->make<MethodDecl>(
            name,
            modifiers,
            returnType,
            parameters,
            block,
            typeParameters,
            returnTypeSourceRange.has_value() ? returnTypeSourceRange.value() : dummy_source,
            nameSourceRange.has_value() ? nameSourceRange.value() : dummy_source,
            bodyRange.has_value() ? bodyRange.value() : dummy_source);
    }

    MethodDeclBuilder& with(Modifier modifier, const SourceRange &sourceRange = dummy_source) {
        modifiers.push_back(arena->make<ModifierNode>(modifier, sourceRange));
        return *this;
    }

    MethodDeclBuilder& with(const std::vector<ModifierNode*>& modifiers) {
        this->modifiers.insert(this->modifiers.end(), modifiers.begin(), modifiers.end());
        return *this;
    }

    MethodDeclBuilder& with(const std::vector<Modifier>& modifiers) {
        return with(modifiers, std::vector(modifiers.size(), dummy_source));
    }

    MethodDeclBuilder& with(const std::vector<Modifier>& modifiers, const std::vector<SourceRange>& sourceRanges) {
        assert(modifiers.size() == sourceRanges.size());
        for (int i = 0; i < modifiers.size(); i++) {
            this->modifiers.push_back(arena->make<ModifierNode>(modifiers[i], sourceRanges[i]));
        }
        return *this;
    }

    MethodDeclBuilder& with(const std::pair<TypeRef*, std::string> &parameter) {
        parameters.push_back(arena->make<FieldDecl>(parameter.second, std::vector<ModifierNode*>{}, parameter.first, dummy_source, dummy_source, dummy_source, nullptr));
        return *this;
    }

    MethodDeclBuilder& with(const std::vector<FieldDecl*>& parameters) {
        this->parameters.insert(this->parameters.begin(), parameters.begin(), parameters.end());
        return *this;
    }

    MethodDeclBuilder& with(FieldDecl* parameter) {
        parameters.push_back(parameter);
        return *this;
    }

    MethodDeclBuilder& with(Block* block) {
        this->block = block;
        return *this;
    }

    MethodDeclBuilder& with(TypeParameterDecl* typeParameter) {
        typeParameters.push_back(typeParameter);
        return *this;
    }

    MethodDeclBuilder& with(const std::vector<TypeParameterDecl*>& typeParameters) {
        this->typeParameters.insert(this->typeParameters.end(), typeParameters.begin(), typeParameters.end());
        return *this;
    }

    MethodDeclBuilder& withReturnTypeSourceRange(const SourceRange& returnTypeSourceRange) {
        this->returnTypeSourceRange.emplace(returnTypeSourceRange);
        return *this;
    }

    MethodDeclBuilder& withNameSourceRange(const SourceRange& nameSourceRange) {
        this->nameSourceRange.emplace(nameSourceRange);
        return *this;
    }

    MethodDeclBuilder& withBodyRange(const SourceRange& bodyRange) {
        this->bodyRange.emplace(bodyRange);
        return *this;
    }

private:
    std::string name;
    std::vector<ModifierNode*> modifiers;;
    TypeRef* returnType;
    std::vector<FieldDecl*> parameters;
    Block* block;
    std::vector<TypeParameterDecl*> typeParameters;

    std::optional<SourceRange> returnTypeSourceRange;
    std::optional<SourceRange> nameSourceRange;
    std::optional<SourceRange> bodyRange;
};


#endif //METHODDECL_H
