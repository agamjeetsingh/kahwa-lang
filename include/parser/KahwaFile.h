//
// Created by Agamjeet Singh on 03/12/25.
//

#ifndef KAHWAFILE_H
#define KAHWAFILE_H
#include <vector>

#include "TypedefDecl.h"


class KahwaFile {
public:
    explicit KahwaFile(const std::vector<TypedefDecl*> &typedefDecls = {}): typedefDecls(typedefDecls) {}

private:
    std::vector<TypedefDecl*> typedefDecls;
};



#endif //KAHWAFILE_H
