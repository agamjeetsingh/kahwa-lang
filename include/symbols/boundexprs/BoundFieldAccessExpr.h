//
// Created by Agamjeet Singh on 28/12/25.
//

#ifndef BOUNDFIELDACCESSEXPR_H
#define BOUNDFIELDACCESSEXPR_H
#include "BoundExpr.h"


struct FieldSymbol;

struct BoundFieldAccessExpr : BoundExpr {
    BoundFieldAccessExpr(BoundExpr* receiver, FieldSymbol* field):
    BoundExpr(field->type, BoundExprKind::FIELD_ACCESS_EXPR),
    receiver(receiver),
    field(field) {}

    BoundExpr* receiver;
    FieldSymbol* field;
};



#endif //BOUNDFIELDACCESSEXPR_H
