//
// Created by Agamjeet Singh on 28/11/25.
//

#ifndef MODIFIER_H
#define MODIFIER_H
#include "../tokeniser/TokenType.h"
#include <stdexcept>
#include "../tokeniser/Token.h"

enum class Modifier {
    OPEN,
    FINAL,
    ABSTRACT,
    PUBLIC,
    PRIVATE,
    PROTECTED,
    STATIC,
    OVERRIDE,
};

inline bool isModalityModifier(Modifier modifier) {
    return modifier == Modifier::OPEN || modifier == Modifier::FINAL || modifier == Modifier::ABSTRACT;
}

inline bool isVisibilityModifier(Modifier modifier) {
    return modifier == Modifier::PUBLIC || modifier == Modifier::PROTECTED || modifier == Modifier::PRIVATE;
}

inline Modifier tokenTypeToModifier(TokenType tokenType) {
    switch (tokenType) {
        case TokenType::OPEN: return Modifier::OPEN;
        case TokenType::FINAL: return Modifier::FINAL;
        case TokenType::ABSTRACT: return Modifier::ABSTRACT;
        case TokenType::PUBLIC: return Modifier::PUBLIC;
        case TokenType::PRIVATE: return Modifier::PRIVATE;
        case TokenType::PROTECTED: return Modifier::PROTECTED;
        case TokenType::STATIC: return Modifier::STATIC;
        case TokenType::OVERRIDE: return Modifier::OVERRIDE;
        default:
            throw std::invalid_argument("TokenType is not a modifier");
    }
}

inline std::string toString(const Modifier modifier) {
    switch (modifier) {
        case Modifier::OPEN: return tokenTypeToString(TokenType::OPEN);
        case Modifier::FINAL: return tokenTypeToString(TokenType::FINAL);
        case Modifier::ABSTRACT: return tokenTypeToString(TokenType::ABSTRACT);
        case Modifier::PUBLIC: return tokenTypeToString(TokenType::PUBLIC);
        case Modifier::PRIVATE: return tokenTypeToString(TokenType::PRIVATE);
        case Modifier::PROTECTED: return tokenTypeToString(TokenType::PROTECTED);
        case Modifier::STATIC: return tokenTypeToString(TokenType::STATIC);
        default: throw std::invalid_argument("Modifier not recognised.");
    }
}

#endif //MODIFIER_H
