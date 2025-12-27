//
// Created by Agamjeet Singh on 26/12/25.
//

#ifndef RETURNSTMT_H
#define RETURNSTMT_H
#include "../expr/Expr.h"
#include "../expr/Stmt.h"


struct ReturnStmt : Stmt {
    Expr* expr;
};



#endif //RETURNSTMT_H
