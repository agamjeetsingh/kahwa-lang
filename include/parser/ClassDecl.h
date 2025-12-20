//
// Created by Agamjeet Singh on 23/11/25.
//

#ifndef CLASS_H
#define CLASS_H

#include <utility>

#include "ASTBuilder.h"
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
        const std::vector<ModifierNode> &modifiers = {},
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

class ClassDeclBuilder : public ASTBuilder {
    public:
        explicit ClassDeclBuilder(std::string  name): name(std::move(name)) {}

        [[nodiscard]] ClassDecl* build() const {
            return arena->make<ClassDecl>(
                name,
                classSourceRange.has_value() ? classSourceRange.value() : dummy_source,
                nameSourceRange.has_value() ? nameSourceRange.value() : dummy_source,
                bodyRange.has_value() ? bodyRange.value() : dummy_source,
                modifiers,
                superClasses,
                fields,
                methods,
                nestedClasses);
        }

        ClassDeclBuilder& with(const std::vector<Modifier> &modifiers) {
            return with(modifiers, std::vector(modifiers.size(), dummy_source));
        }

        ClassDeclBuilder& with(Modifier modifier, const SourceRange &sourceRange = dummy_source) {
            modifiers.emplace_back(modifier, sourceRange);
            return *this;
        }

        ClassDeclBuilder& with(const std::vector<Modifier> &modifiers, const std::vector<SourceRange>& sourceRanges) {
            assert(modifiers.size() == sourceRanges.size());
            for (int i = 0; i < modifiers.size(); i++) {
                this->modifiers.emplace_back(modifiers[i], sourceRanges[i]);
            }
            return *this;
        }

        ClassDeclBuilder& with(TypeRef* superClass) {
            superClasses.push_back(superClass);
            return *this;
        }

        ClassDeclBuilder& with(std::vector<TypeRef*> superClasses) {
            this->superClasses.insert(this->superClasses.end(), superClasses.begin(), superClasses.end());
            return *this;
        }

        ClassDeclBuilder& with(FieldDecl* field) {
            fields.push_back(field);
            return *this;
        }

        ClassDeclBuilder& with(std::vector<FieldDecl*> fields) {
            this->fields.insert(this->fields.end(), fields.begin(), fields.end());
            return *this;
        }

        ClassDeclBuilder& with(MethodDecl* method) {
            methods.push_back(method);
            return *this;
        }

        ClassDeclBuilder& with(std::vector<MethodDecl*> methods) {
            this->methods.insert(this->methods.end(), methods.begin(), methods.end());
            return *this;
        }

        ClassDeclBuilder& with(ClassDecl* nestedClass) {
            nestedClasses.push_back(nestedClass);
            return *this;
        }

        ClassDeclBuilder& with(std::vector<ClassDecl*> nestedClasses) {
            this->nestedClasses.insert(this->nestedClasses.end(), nestedClasses.begin(), nestedClasses.end());
            return *this;
        }

        ClassDeclBuilder& withClassSourceRange(const SourceRange& classSourceRange) {
            this->classSourceRange.emplace(classSourceRange);
            return *this;
        }

        ClassDeclBuilder& withNameSourceRange(const SourceRange& nameSourceRange) {
            this->nameSourceRange.emplace(nameSourceRange);
            return *this;
        }

        ClassDeclBuilder& withBodyRange(const SourceRange& bodyRange) {
            this->bodyRange.emplace(bodyRange);
            return *this;
        }

    private:
        std::string name;
        std::vector<ModifierNode> modifiers;
        std::vector<TypeRef*> superClasses;
        std::vector<FieldDecl*> fields;
        std::vector<MethodDecl*> methods;
        std::vector<ClassDecl*> nestedClasses;
        std::optional<SourceRange> classSourceRange;
        std::optional<SourceRange> nameSourceRange;
        std::optional<SourceRange> bodyRange;
    };



#endif //CLASS_H
