//
// Created by Agamjeet Singh on 10/12/25.
//

#ifndef ASTBUILDER_H
#define ASTBUILDER_H
#include "../arena/Arena.h"
#include "../source/SourceRange.h"


class ASTBuilder {
public:
    static void setArena(Arena* arena) { ASTBuilder::arena = arena; }

protected:
    static Arena* arena;
    static SourceRange dummy_source;
};



#endif //ASTBUILDER_H
