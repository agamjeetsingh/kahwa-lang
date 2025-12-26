//
// Created by Agamjeet Singh on 03/12/25.
//

#ifndef KAHWAFILE_H
#define KAHWAFILE_H
#include <vector>

#include "ASTBuilder.h"
#include "TypedefDecl.h"
#include "../arena/Arena.h"
#include "ClassDecl.h"

struct KahwaFile {
    explicit KahwaFile(const std::vector<TypedefDecl*> &typedefDecls = {},
        const std::vector<ClassDecl*> &classDecls = {},
        const std::vector<MethodDecl*> &functionDecls = {},
        const std::vector<FieldDecl*> &variableDecls = {}):
    typedefDecls(typedefDecls),
    classDecls(classDecls),
    functionDecls(functionDecls),
    variableDecls(variableDecls) {}

    const std::vector<TypedefDecl*> typedefDecls;
    const std::vector<ClassDecl*> classDecls;
    const std::vector<MethodDecl*> functionDecls;
    const std::vector<FieldDecl*> variableDecls;

    bool operator==(const KahwaFile &other) const {
        if (typedefDecls.size() != other.typedefDecls.size() ||
            classDecls.size() != other.classDecls.size() ||
            functionDecls.size() != other.functionDecls.size() ||
            variableDecls.size() != other.variableDecls.size()) {
            return false;
        }
        
        for (size_t i = 0; i < typedefDecls.size(); ++i) {
            if (typedefDecls[i] == nullptr && other.typedefDecls[i] == nullptr) continue;
            if (typedefDecls[i] == nullptr || other.typedefDecls[i] == nullptr) return false;
            if (*typedefDecls[i] != *other.typedefDecls[i]) return false;
        }
        for (size_t i = 0; i < classDecls.size(); ++i) {
            if (classDecls[i] == nullptr && other.classDecls[i] == nullptr) continue;
            if (classDecls[i] == nullptr || other.classDecls[i] == nullptr) return false;
            if (*classDecls[i] != *other.classDecls[i]) return false;
        }
        for (size_t i = 0; i < functionDecls.size(); ++i) {
            if (functionDecls[i] == nullptr && other.functionDecls[i] == nullptr) continue;
            if (functionDecls[i] == nullptr || other.functionDecls[i] == nullptr) return false;
            if (*functionDecls[i] != *other.functionDecls[i]) return false;
        }
        for (size_t i = 0; i < variableDecls.size(); ++i) {
            if (variableDecls[i] == nullptr && other.variableDecls[i] == nullptr) continue;
            if (variableDecls[i] == nullptr || other.variableDecls[i] == nullptr) return false;
            if (*variableDecls[i] != *other.variableDecls[i]) return false;
        }
        
        return true;
    }
};

class KahwaFileBuilder : public ASTBuilder {
public:
    KahwaFileBuilder() = default;

    [[nodiscard]] KahwaFile* build() const {
        return arena->make<KahwaFile>(typedefDecls, classDecls, functionDecls, variableDecls);
    }

    KahwaFileBuilder& with(TypedefDecl* typedefDecl) {
        typedefDecls.push_back(typedefDecl);
        return *this;
    }

    KahwaFileBuilder& with(std::vector<TypedefDecl*> typedefDecls) {
        this->typedefDecls.insert(this->typedefDecls.end(), typedefDecls.begin(), typedefDecls.end());
        return *this;
    }

    KahwaFileBuilder& with(ClassDecl* classDecl) {
        classDecls.push_back(classDecl);
        return *this;
    }

    KahwaFileBuilder& with(std::vector<ClassDecl*> classDecls) {
        this->classDecls.insert(this->classDecls.end(), classDecls.begin(), classDecls.end());
        return *this;
    }

    KahwaFileBuilder& with(MethodDecl* functionDecl) {
        functionDecls.push_back(functionDecl);
        return *this;
    }

    KahwaFileBuilder& with(std::vector<MethodDecl*> functionDecls) {
        this->functionDecls.insert(this->functionDecls.end(), functionDecls.begin(), functionDecls.end());
        return *this;
    }

    KahwaFileBuilder& with(FieldDecl* variableDecl) {
        variableDecls.push_back(variableDecl);
        return *this;
    }

    KahwaFileBuilder& with(std::vector<FieldDecl*> variableDecls) {
        this->variableDecls.insert(this->variableDecls.end(), variableDecls.begin(), variableDecls.end());
        return *this;
    }

private:
    std::vector<TypedefDecl*> typedefDecls;
    std::vector<ClassDecl*> classDecls;
    std::vector<MethodDecl*> functionDecls;
    std::vector<FieldDecl*> variableDecls;
};



#endif //KAHWAFILE_H
