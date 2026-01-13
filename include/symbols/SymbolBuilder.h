//
// Created by Agamjeet Singh on 19/12/25.
//

#ifndef SYMBOLBUILDER_H
#define SYMBOLBUILDER_H

#include "../arena/Arena.h"

class SymbolBuilder {
public:
    static void setArena(Arena* arena) { SymbolBuilder::arena = arena; }

private:
    inline static Arena demoArena;
protected:
    inline static Arena* arena = &demoArena;

};



#endif //SYMBOLBUILDER_H
