//
// Created by Agamjeet Singh on 26/12/25.
//

#ifndef BOOLLITERAL_H
#define BOOLLITERAL_H



struct BoolLiteral : Expr {
    BoolLiteral(bool val, const SourceRange& bodyRange): Expr(bodyRange, ExprKind::BOOL_LITERAL), val(val) {}

    bool val;
};



#endif //BOOLLITERAL_H
