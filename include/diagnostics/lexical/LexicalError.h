//
// Created by Agamjeet Singh on 24/11/25.
//

#ifndef LEXICALERROR_H
#define LEXICALERROR_H
#include "../CompilationError.h"


class LexicalError : public CompilationError {
public:
    explicit LexicalError(std::string msg) : CompilationError(std::move(msg)) {}
};



#endif //LEXICALERROR_H
