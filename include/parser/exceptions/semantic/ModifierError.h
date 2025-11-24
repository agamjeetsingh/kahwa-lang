//
// Created by Agamjeet Singh on 23/11/25.
//

#ifndef CLASSMODIFIEREXCEPTION_H
#define CLASSMODIFIEREXCEPTION_H
#include <string>
#include <utility>

#include "SemanticError.h"
#include "../CompilationError.h"


class ModifierError : public SemanticError {
public:
    explicit ModifierError(std::string msg): SemanticError(std::move(msg)) {}
};

class IllegalModifierCombinationError final : public ModifierError {
public:
    IllegalModifierCombinationError(std::string modifier1, std::string modifier2): ModifierError("Illegal combination of modifiers '" + std::move(modifier1) + "' and '" + std::move(modifier2) + "'") {}
};

class RepeatedModifierError final : public ModifierError {
public:
    explicit RepeatedModifierError(std::string repeatedModifier): ModifierError("Repeated modifier: '" + std::move(repeatedModifier) + "'") {}
};


#endif //CLASSMODIFIEREXCEPTION_H
