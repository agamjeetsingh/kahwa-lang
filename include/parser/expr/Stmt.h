//
// Created by Agamjeet Singh on 04/12/25.
//

#ifndef STMT_H
#define STMT_H
#include "../../source/SourceRange.h"


struct Stmt {
    bool operator==(const Stmt &other) const {
        return true; // Empty struct, all instances are equal
    }

    const SourceRange bodyRange;
};



#endif //STMT_H
