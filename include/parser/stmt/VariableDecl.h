//
// Created by Agamjeet Singh on 02/01/26.
//

#ifndef VARIABLEDECL_H
#define VARIABLEDECL_H
#include "../FieldDecl.h"
#include "../expr/Stmt.h"


struct VariableDecl : Stmt {
    explicit VariableDecl(FieldDecl* decl):
    Stmt(decl->bodyRange, StmtKind::VARIABLE_DECL),
    decl(decl) {}
    FieldDecl* decl;
};



#endif //VARIABLEDECL_H
