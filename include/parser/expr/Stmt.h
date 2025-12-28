//
// Created by Agamjeet Singh on 04/12/25.
//

#ifndef STMT_H
#define STMT_H
#include "../../source/SourceRange.h"
#include "../stmt/StmtKind.h"


struct Stmt {
    explicit Stmt(const SourceRange& bodyRange, const StmtKind kind = StmtKind::STMT): bodyRange(bodyRange), kind(kind) {}

    bool operator==(const Stmt &other) const {
        return true; // Empty struct, all instances are equal
    }

    virtual ~Stmt() noexcept = default;

    const SourceRange bodyRange;

    const StmtKind kind;
};



#endif //STMT_H
