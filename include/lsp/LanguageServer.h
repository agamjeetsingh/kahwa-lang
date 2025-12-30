//
// Created by Agamjeet Singh on 30/12/25.
//

#ifndef LANGUAGESERVER_H
#define LANGUAGESERVER_H
#include "../source/SourceManager.h"

#include <nlohmann/json.hpp>

#include "LSPTokenModifier.h"
#include "LSPTokenType.h"
#include "../arena/Arena.h"
#include "../parser/Parser.h"
#include "../tokeniser/Tokeniser.h"
using json = nlohmann::json;

class LanguageServer {
public:
    LanguageServer() {
        for (int i = 0; i < magic_enum::enum_values<LSPTokenType>().size(); i++) {
            tokenTypeIdx[magic_enum::enum_values<LSPTokenType>()[i]] = i;
        }

        for (int i = 0; i < magic_enum::enum_values<LSPTokenModifier>().size(); i++) {
            tokenModifierIdx[magic_enum::enum_values<LSPTokenModifier>()[i]] = i;
        }
    }

    void addFile(const std::string& fileContents, const std::string& uri) {
        if (uriToId.contains(uri)) {
            sourceManager.removeFile(uriToId[uri]);
        }

        auto id = sourceManager.addFile(fileContents);
        idToUri[id] = uri;
        uriToId[uri] = id;
    }

    void removeFile(const std::string& uri) {
        if (!uriToId.contains(uri)) return;

        auto id = uriToId[uri];
        idToUri.erase(id);
        uriToId.erase(uri);
    }

    std::vector<int> syntaxHighlight(const std::string& uri) {
        if (!uriToId.contains(uri)) {
            return {};
        }

        std::string fileContent = sourceManager.getSource(uriToId[uri]);

        const auto& tokens = tokeniser.tokenise(uriToId[uri], fileContent);

        parser.parseFile(tokens);

        std::vector<std::tuple<Token, std::optional<LSPTokenType>, std::vector<LSPTokenModifier>>> tokensWithData;

        // Somehow get tokenWithData
        std::ranges::for_each(tokens, [&tokensWithData](const Token& token) {
            if (KEYWORD_TYPES.contains(token.type)) {
                tokensWithData.push_back({token, LSPTokenType::KEYWORD, {}});
            }
        });

        std::vector<int> res;

        int prevLine = 0;
        int prevPos = 0;

        SourceFile& file = sourceManager.getSourceFile(uriToId[uri]);

        std::ranges::for_each(tokensWithData, [&prevLine, &prevPos, &file, &res, this](const std::tuple<Token, std::optional<LSPTokenType>, std::vector<LSPTokenModifier>>& tuple) {
            if (!std::get<1>(tuple)) return;

            const Token& tok = std::get<0>(tuple);

            const auto& linePos = file.getLinePos(tok.source_range.pos);

            int currentLine = linePos.first;
            int currentPos = linePos.second;

            int deltaLine = currentLine - prevLine;
            res.push_back(deltaLine);
            int deltaStart = deltaLine == 0 ? currentPos - prevPos : currentPos;
            res.push_back(deltaStart);

            prevLine = currentLine;
            prevPos = currentPos;

            res.push_back(tok.source_range.length);

            res.push_back(tokenTypeIdx[std::get<1>(tuple).value()]);

            int tokenModifierInt = 0;
            std::ranges::for_each(std::get<2>(tuple), [&tokenModifierInt, this](LSPTokenModifier modifier) {
                tokenModifierInt |= 1 << tokenModifierIdx[modifier];
            });

            res.push_back(tokenModifierInt);
        });

        return res;
    }

private:
    SourceManager sourceManager;
    DiagnosticEngine diagnosticEngine;
    Arena arena;
    Tokeniser tokeniser{diagnosticEngine};
    Parser parser{arena, diagnosticEngine};

    std::unordered_map<LSPTokenType, int> tokenTypeIdx;
    std::unordered_map<LSPTokenModifier, int> tokenModifierIdx;

    std::unordered_map<std::string, std::size_t> uriToId;
    std::unordered_map<std::size_t, std::string> idToUri;
};



#endif //LANGUAGESERVER_H
