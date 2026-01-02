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
#include "../symbols/SemanticAnalyser.h"
#include "../tokeniser/Tokeniser.h"
class LSPConnection;
using json = nlohmann::json;

typedef std::tuple<SourceRange, LSPTokenType, std::vector<LSPTokenModifier>> tokenData;

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

    void setConnection(LSPConnection* connection) {
        lspConnection = connection;
    }

    void addFile(const std::string& fileContents, const std::string& uri) {
        // if (uriToId.contains(uri)) {
        //     sourceManager.removeFile(uriToId[uri]);
        // }

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

    std::vector<int> syntaxHighlight(const std::string& uri);

private:
    SourceManager sourceManager;
    DiagnosticEngine diagnosticEngine;
    Arena arena;
    Tokeniser tokeniser{diagnosticEngine};
    Parser parser{arena, diagnosticEngine};
    LSPConnection* lspConnection;
    SemanticAnalyser semanticAnalyser{arena, diagnosticEngine};

    std::unordered_map<LSPTokenType, int> tokenTypeIdx;
    std::unordered_map<LSPTokenModifier, int> tokenModifierIdx;

    std::unordered_map<std::string, std::size_t> uriToId;
    std::unordered_map<std::size_t, std::string> idToUri;
};



#endif //LANGUAGESERVER_H
