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
#include "../source/SourceManager.h"

class Tokeniser {
public:
    explicit Tokeniser(DiagnosticEngine& diagnostic_engine): diagnostic_engine(diagnostic_engine) {}

    [[nodiscard]] std::vector<Token> tokenise(const std::size_t file_id, const std::string_view str) const {
        return TokeniserWorker(file_id, str, diagnostic_engine).tokenise();
    }

    class TokeniserWorker {
    public:
        TokeniserWorker(const std::size_t file_id, const std::string_view str, DiagnosticEngine& diagnostic_engine): file_id(file_id), str(str), diagnostic_engine(diagnostic_engine) {}

        std::vector<Token> tokenise();
    private:
        std::size_t idx = 0;

        std::vector<Token> tokens;

        const std::size_t file_id;
        const std::string_view str;
        DiagnosticEngine& diagnostic_engine;

        std::optional<Token> tokeniseString(std::size_t curr_idx);

        std::string getNumberString(std::size_t curr_idx);

        std::string extractIdentifierLike();

        bool next_is(const std::string& expected, const std::function<bool(char)>& until = [](char c){ return false; }) const;

        std::string next(const std::function<bool(char)>& until) const;

        std::string next(std::size_t count = 1, const std::function<bool(char)>& until = [](char c){ return false; }) const;

        static const std::unordered_set<char> DELIMITERS;

        static const std::unordered_map<std::string, TokenType> TOKEN_MAP;
    };

private:
    DiagnosticEngine& diagnostic_engine;
};



#endif //TOKENISER_H
