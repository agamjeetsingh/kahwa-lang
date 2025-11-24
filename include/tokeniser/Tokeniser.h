//
// Created by Agamjeet Singh on 22/11/25.
//

#ifndef TOKENISER_H
#define TOKENISER_H
#include <assert.h>
#include <iostream>
#include <ostream>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

#include "Token.h"

class Tokeniser {
public:
    explicit Tokeniser(std::string str): str(std::move(str)) {}

    std::optional<std::vector<Token>> tokenise();

    const std::string str;

    std::vector<Token> tokens;

    std::unordered_set<char> delimiters = {' ', '\t', '\r', '\n', '\f'};
private:
    std::size_t idx = 0;

    std::optional<Token> tokeniseString(std::size_t curr_idx);

    Token tokeniseNumber(std::size_t curr_idx);

    std::string extractIdentifierLike();

    static bool isStartOfIdentifier(char c) {
        return std::isalpha(c) || c == '_';
    }

    bool next_is(const std::string& expected, const std::function<bool(char)>& until = [](char c){ return false; }) const;

    std::string next(const std::function<bool(char)>& until) const;

    std::string next(std::size_t count = 1, const std::function<bool(char)>& until = [](char c){ return false; }) const;

    const std::unordered_map<std::string, TokenType> tokenMap{
        {"class", TokenType::CLASS},
        {"static", TokenType::STATIC},
        {"public", TokenType::PUBLIC},
        {"private", TokenType::PRIVATE},
        {"protected", TokenType::PROTECTED},
        {"open", TokenType::OPEN},
        {"final", TokenType::FINAL},
        {"abstract", TokenType::ABSTRACT},
        {"interface", TokenType::INTERFACE},
        {"typedef", TokenType::TYPEDEF},
        {"return", TokenType::RETURN},
        {"if", TokenType::IF},
        {"else", TokenType::ELSE},
        {"for", TokenType::FOR},
        {"while", TokenType::WHILE},
        {"break", TokenType::BREAK},
        {"continue", TokenType::CONTINUE},
        {"true", TokenType::TRUE},
        {"false", TokenType::FALSE},
        {"null", TokenType::NULL_LITERAL}
    };
};



#endif //TOKENISER_H
