//
// Created by Agamjeet Singh on 26/12/25.
//

#ifndef BREAKSTMT_H
#define BREAKSTMT_H
#include "../expr/Stmt.h"


struct BreakStmt : Stmt {
    explicit BreakStmt(const SourceRange& bodyRange): Stmt(bodyRange) {}
};



#endif //BREAKSTMT_H
