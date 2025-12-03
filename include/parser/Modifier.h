//
// Created by Agamjeet Singh on 28/11/25.
//

#ifndef MODIFIER_H
#define MODIFIER_H

enum class Modifier {
    OPEN,
    FINAL,
    ABSTRACT,
    PUBLIC,
    PRIVATE,
    PROTECTED,
    STATIC
};

inline Modifier tokenTypeToModifier(TokenType tokenType) {
    switch (tokenType) {
        case TokenType::OPEN: return Modifier::OPEN;
        case TokenType::FINAL: return Modifier::FINAL;
        case TokenType::ABSTRACT: return Modifier::ABSTRACT;
        case TokenType::PUBLIC: return Modifier::PUBLIC;
        case TokenType::PRIVATE: return Modifier::PRIVATE;
        case TokenType::PROTECTED: return Modifier::PROTECTED;
        case TokenType::STATIC: return Modifier::STATIC;
        default:
            throw std::invalid_argument("TokenType is not a modifier");
    }
}

#endif //MODIFIER_H
