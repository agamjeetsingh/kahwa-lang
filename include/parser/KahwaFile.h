//
// Created by Agamjeet Singh on 03/12/25.
//

#ifndef KAHWAFILE_H
#define KAHWAFILE_H
#include <vector>

#include "TypedefDecl.h"


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



#endif //KAHWAFILE_H
