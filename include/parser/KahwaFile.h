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
};



#endif //KAHWAFILE_H
