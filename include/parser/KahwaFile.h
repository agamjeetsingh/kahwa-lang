//
// Created by Agamjeet Singh on 03/12/25.
//

#ifndef KAHWAFILE_H
#define KAHWAFILE_H
#include <vector>

#include "TypedefDecl.h"


struct KahwaFile {
    explicit KahwaFile(const std::vector<TypedefDecl*> &typedefDecls = {},
        const std::vector<ClassDecl*> &classDecls = {}
        ): typedefDecls(typedefDecls),
           classDecls(classDecls) {}

    const std::vector<TypedefDecl*> typedefDecls;
    const std::vector<ClassDecl*> classDecls;
};



#endif //KAHWAFILE_H
