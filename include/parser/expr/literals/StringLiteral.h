//
// Created by Agamjeet Singh on 26/12/25.
//

#ifndef STRINGLITERAL_H
#define STRINGLITERAL_H
#include <string>

#include "../Expr.h"


struct StringLiteral : Expr {
    std::string val;
};



#endif //STRINGLITERAL_H
