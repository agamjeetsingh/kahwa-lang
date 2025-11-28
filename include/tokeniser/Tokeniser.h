//
// Created by Agamjeet Singh on 22/11/25.
//

#ifndef TOKENISER_H
#define TOKENISER_H

#include <ostream>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

#include "Token.h"
#include "../diagnostics/DiagnosticEngine.h"

class Tokeniser {
public:
    Tokeniser(std::string str, std::shared_ptr<DiagnosticEngine> diagnostic_engine): str(std::move(str)), diagnostic_engine(diagnostic_engine) {}

    std::optional<std::vector<Token>> tokenise();
private:
    std::size_t idx = 0;

    std::vector<Token> tokens;

    const std::string str;
    const std::shared_ptr<DiagnosticEngine>& diagnostic_engine;

    std::optional<Token> tokeniseString(std::size_t curr_idx);

    Token tokeniseNumber(std::size_t curr_idx);

    std::string extractIdentifierLike();

    bool next_is(const std::string& expected, const std::function<bool(char)>& until = [](char c){ return false; }) const;

    std::string next(const std::function<bool(char)>& until) const;

    std::string next(std::size_t count = 1, const std::function<bool(char)>& until = [](char c){ return false; }) const;

    static const std::unordered_set<char> DELIMITERS;

    static const std::unordered_map<std::string, TokenType> TOKEN_MAP;
};



#endif //TOKENISER_H
