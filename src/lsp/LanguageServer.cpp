//
// Created by Agamjeet Singh on 30/12/25.
//

#include "../../include/lsp/LanguageServer.h"
#include "../../include/lsp/LSPConnection.h"
#include "../../include/lsp/SyntaxHighlightingVisitor.h"

std::vector<tokenData> LanguageServer::syntaxHighlight(const std::string &content, std::size_t file_id) {
    const auto& tokens = tokeniser.tokenise(file_id, content);

    auto kahwaFile = parser.parseFile(tokens);


    if (!diagnosticEngine.getAll().empty()) {
        lspConnection->publishDiagnostics(sourceManager, diagnosticEngine.getAll(file_id), idToUri);
    }

    std::vector<tokenData> tokensWithData;
    semanticAnalyser.declareFile(kahwaFile);

    auto commentTokens = tokeniser.getComments(file_id, content);
    std::ranges::for_each(commentTokens, [&tokensWithData](const SourceRange& sourceRange) {
       tokensWithData.emplace_back(sourceRange, LSPTokenType::COMMENT, std::vector<LSPTokenModifier>{});
    });

    std::ranges::for_each(tokens, [&tokensWithData](const Token& token) {
        if (KEYWORD_TYPES.contains(token.type)) {
            tokensWithData.emplace_back(token.source_range, LSPTokenType::KEYWORD, std::vector<LSPTokenModifier>{});
        } else if (token.type == TokenType::INTEGER || token.type == TokenType::FLOAT) {
            tokensWithData.emplace_back(token.source_range, LSPTokenType::NUMBER, std::vector<LSPTokenModifier>{});
        } else if (token.type == TokenType::STRING_LITERAL) {
            tokensWithData.emplace_back(token.source_range, LSPTokenType::STRING, std::vector<LSPTokenModifier>{});
        }
    });

    SyntaxHighlightingVisitor(tokensWithData, semanticAnalyser).visit(kahwaFile);

    std::ranges::sort(tokensWithData, [](const auto& t1, const auto& t2) {
       return std::get<0>(t1).pos < std::get<0>(t2).pos;
    });

    return tokensWithData;
}

std::vector<int> LanguageServer::syntaxHighlight(const std::string &uri) {
    if (!uriToId.contains(uri)) {
        return {};
    }

    std::string fileContent = sourceManager.getSource(uriToId[uri]);

    return highlightingDataToLspFormat(syntaxHighlight(fileContent, uriToId[uri]), uriToId[uri]);
}

std::vector<int> LanguageServer::highlightingDataToLspFormat(const std::vector<tokenData>& data, std::size_t file_id) {
    std::vector<int> res;

    int prevLine = 0;
    int prevPos = 0;

    SourceFile& file = sourceManager.getSourceFile(file_id);

    std::ranges::for_each(data, [&prevLine, &prevPos, &file, &res, this](const auto& tuple) {
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
