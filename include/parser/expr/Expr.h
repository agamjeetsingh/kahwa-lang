//
// Created by Agamjeet Singh on 26/12/25.
//

#ifndef EXPR_H
#define EXPR_H
#include "Stmt.h"
#include "../../source/SourceRange.h"


struct Expr : Stmt {
    const SourceRange bodyRange;
};



#endif //EXPR_H
