//
// Created by Agamjeet Singh on 26/12/25.
//

#ifndef CONTINUESTMT_H
#define CONTINUESTMT_H
#include "../expr/Stmt.h"


struct ContinueStmt : Stmt {
    explicit ContinueStmt(const SourceRange& bodyRange): Stmt(bodyRange) {}
};



#endif //CONTINUESTMT_H
