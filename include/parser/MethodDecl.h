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

// Functions and Methods are identical at the AST level (but not in symbols)
struct MethodDecl : Decl {
    MethodDecl(std::string name,
    const std::vector<ModifierNode> &modifiers,
    TypeRef* returnType,
    const std::vector<std::pair<TypeRef*, std::string>>& parameters,
    Block* block,
    const std::vector<TypeRef*> &typeParameters,
    const SourceRange &returnTypeSourceRange,
    const SourceRange &nameSourceRange,
    const SourceRange &bodyRange):
    Decl(std::move(name), modifiers, nameSourceRange, bodyRange),
    returnType(returnType),
    parameters(parameters),
    block(block),
    typeParameters(typeParameters),
    returnTypeSourceRange(returnTypeSourceRange) {}

    TypeRef* const returnType;
    const std::vector<std::pair<TypeRef*, std::string>> parameters;
    Block* const block;

    const std::vector<TypeRef*> typeParameters;

    const SourceRange returnTypeSourceRange;

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
            if (parameters[i].second != other.parameters[i].second) return false;
            
            auto &typeA = parameters[i].first;
            auto &typeB = other.parameters[i].first;
            if (typeA == nullptr && typeB == nullptr) continue;
            if (typeA == nullptr || typeB == nullptr) return false;
            if (*typeA != *typeB) return false;
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

    [[nodiscard]] MethodDecl* build() const {
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
        modifiers.emplace_back(modifier, sourceRange);
        return *this;
    }

    MethodDeclBuilder& with(const std::vector<Modifier>& modifiers) {
        return with(modifiers, std::vector(modifiers.size(), dummy_source));
    }

    MethodDeclBuilder& with(const std::vector<Modifier>& modifiers, const std::vector<SourceRange>& sourceRanges) {
        assert(modifiers.size() == sourceRanges.size());
        for (int i = 0; i < modifiers.size(); i++) {
            this->modifiers.emplace_back(modifiers[i], sourceRanges[i]);
        }
        return *this;
    }

    MethodDeclBuilder& with(const std::pair<TypeRef*, std::string> &parameter) {
        parameters.push_back(parameter);
        return *this;
    }

    MethodDeclBuilder& with(const std::vector<std::pair<TypeRef*, std::string>>& parameters) {
        this->parameters.insert(this->parameters.begin(), parameters.begin(), parameters.end());
        return *this;
    }

    MethodDeclBuilder& with(Block* block) {
        this->block = block;
        return *this;
    }

    MethodDeclBuilder& with(TypeRef* typeParameter) {
        typeParameters.push_back(typeParameter);
        return *this;
    }

    MethodDeclBuilder& with(const std::vector<TypeRef*>& typeParameters) {
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
    std::vector<ModifierNode> modifiers;;
    TypeRef* returnType;
    std::vector<std::pair<TypeRef*, std::string>> parameters;
    Block* block;
    std::vector<TypeRef*> typeParameters;

    std::optional<SourceRange> returnTypeSourceRange;
    std::optional<SourceRange> nameSourceRange;
    std::optional<SourceRange> bodyRange;
};


#endif //METHODDECL_H
