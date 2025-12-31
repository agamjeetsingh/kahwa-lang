//
// Created by Agamjeet Singh on 30/12/25.
//

#include "LanguageServer.h"
#include "LSPConnection.h"

std::vector<int> LanguageServer::syntaxHighlight(const std::string &uri) {
    if (!uriToId.contains(uri)) {
        return {};
    }

    std::string fileContent = sourceManager.getSource(uriToId[uri]);

    const auto& tokens = tokeniser.tokenise(uriToId[uri], fileContent);

    auto kahwaFile = parser.parseFile(tokens);

    if (!diagnosticEngine.getAll().empty()) {
        lspConnection->publishDiagnostics(sourceManager, diagnosticEngine.getAll(), idToUri);
    }

    std::vector<tokenData> tokensWithData;

    // Somehow get tokenWithData
    std::ranges::for_each(tokens, [&tokensWithData](const Token& token) {
        if (KEYWORD_TYPES.contains(token.type)) {
            tokensWithData.push_back({token.source_range, LSPTokenType::KEYWORD, {}});
        }
    });



    std::ranges::sort(tokensWithData, [](const auto& t1, const auto& t2) {
       return std::get<0>(t1).pos < std::get<0>(t2).pos;
    });

    std::vector<int> res;

    int prevLine = 0;
    int prevPos = 0;

    SourceFile& file = sourceManager.getSourceFile(uriToId[uri]);

    std::ranges::for_each(tokensWithData, [&prevLine, &prevPos, &file, &res, this](const auto& tuple) {
        const SourceRange& sourceRange = std::get<0>(tuple);

        const auto& linePos = file.getLinePos(sourceRange.pos);

        int currentLine = linePos.first;
        int currentPos = linePos.second;

        int deltaLine = currentLine - prevLine;
        res.push_back(deltaLine);
        int deltaStart = deltaLine == 0 ? currentPos - prevPos : currentPos;
        res.push_back(deltaStart);

        prevLine = currentLine;
        prevPos = currentPos;

        res.push_back(sourceRange.length);

        res.push_back(tokenTypeIdx[std::get<1>(tuple)]);

        int tokenModifierInt = 0;
        std::ranges::for_each(std::get<2>(tuple), [&tokenModifierInt, this](LSPTokenModifier modifier) {
            tokenModifierInt |= 1 << tokenModifierIdx[modifier];
        });

        res.push_back(tokenModifierInt);
    });

    return res;
}
