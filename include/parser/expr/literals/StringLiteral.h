//
// Created by Agamjeet Singh on 26/12/25.
//

#ifndef STRINGLITERAL_H
#define STRINGLITERAL_H
#include <string>

#include "../Expr.h"


struct StringLiteral : Expr {
    StringLiteral(std::string val, const SourceRange& bodyRange): Expr(bodyRange), val(val) {}

    std::string val;
};



#endif //STRINGLITERAL_H
