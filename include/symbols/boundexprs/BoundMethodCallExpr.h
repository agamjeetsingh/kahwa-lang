//
// Created by Agamjeet Singh on 28/12/25.
//

#ifndef BOUNDMETHODCALLEXPR_H
#define BOUNDMETHODCALLEXPR_H
#include "BoundExpr.h"


struct BoundMethodCallExpr : BoundExpr {
    BoundMethodCallExpr(BoundExpr* receiver, MethodSymbol* method, const std::vector<BoundExpr*>& args):
    BoundExpr(method->returnType, BoundExprKind::METHOD_CALL_EXPR),
    receiver(receiver),
    method(method),
    args(args) {}

    BoundExpr* receiver; // "this"
    MethodSymbol* method;
    std::vector<BoundExpr*> args;
};



#endif //BOUNDMETHODCALLEXPR_H
