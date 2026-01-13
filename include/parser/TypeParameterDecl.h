//
// Created by Agamjeet Singh on 01/01/26.
//

#ifndef TYPEPARAMETERDECL_H
#define TYPEPARAMETERDECL_H
#include <string>

#include "ASTNode.h"
#include "../types/Variance.h"


struct TypeParameterDecl : ASTNode {
    explicit TypeParameterDecl(const SourceRange& bodyRange, const std::string &name, const Variance variance):
    ASTNode(bodyRange),
    name(name),
    variance(variance) {}

    void visitChildren(ASTVisitor &v) override {}

    void accept(ASTVisitor &v) override {
        v.visit(this);
    }

    std::string name;
    Variance variance;
};



#endif //TYPEPARAMETERDECL_H
