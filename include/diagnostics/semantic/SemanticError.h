//
// Created by Agamjeet Singh on 24/11/25.
//

#ifndef SEMANTICERROR_H
#define SEMANTICERROR_H
#include <utility>

#include "../CompilationError.h"


class SemanticError : public CompilationError {
public:
    explicit SemanticError(std::string msg): CompilationError(std::move(msg)) {}
};



#endif //SEMANTICERROR_H
