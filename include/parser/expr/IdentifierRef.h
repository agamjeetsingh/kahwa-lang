//
// Created by Agamjeet Singh on 26/12/25.
//

#ifndef IDENTIFIERREF_H
#define IDENTIFIERREF_H
#include <string>

#include "Expr.h"


struct IdentifierRef : Expr {
    std::string name;
};



#endif //IDENTIFIERREF_H
