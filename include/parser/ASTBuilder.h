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
    inline static Arena* arena;
    inline static SourceRange dummy_source = {0, 0};
};



#endif //ASTBUILDER_H
