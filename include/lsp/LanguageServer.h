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

    LSPTokenType getClassTokenType(ClassSymbol* classSymbol) {
        return classSymbol->isInterface ? LSPTokenType::CLASS : LSPTokenType::INTERFACE;
    }

    void getSourceRanges(KahwaFile* kahwaFile, std::vector<tokenData>& data) {
        auto translationUnit = semanticAnalyser.processFile(kahwaFile);

        std::ranges::for_each(translationUnit->classes, [&data](const auto* classSymbol) {
           getSourceRanges(classSymbol, data);
        });

        std::ranges::for_each(translationUnit->functions, [&data](const auto* functionSymbol) {
           getSourceRanges(functionSymbol, data);
        });

        std::ranges::for_each(translationUnit->variables, [&data](const auto* variableSymbol) {
           getSourceRanges(variableSymbol, data);
        });

        // TODO - Add type aliases to translation unit

        std::ranges::for_each(kahwaFile->typedefDecls, [&data](const auto* typedefDecl) {
            getSourceRanges(typedefDecl, data);
        });
    }

    void getSourceRanges(TypedefDecl* typedefDecl, std::vector<tokenData>& data) {
        data.emplace_back(typedefDecl->typedefSourceRange, LSPTokenType::KEYWORD, std::vector<LSPTokenModifier>{});
        data.emplace_back(typedefDecl->referredType->bodyRange, LSPTokenType::TYPE, std::vector<LSPTokenModifier>{});
        data.emplace_back(typedefDecl->nameSourceRange, LSPTokenType::TYPE, std::vector{LSPTokenModifier::DECLARATION});

        std::ranges::for_each(typedefDecl->modifiers, [&data](const auto* modifierNode) {
            getSourceRanges(modifierNode, data);
        });
    }

    void getSourceRanges(ClassSymbol* classSymbol, std::vector<tokenData>& data) {
        auto classDecl = dynamic_cast<ClassDecl*>(semanticAnalyser.symbolToASTNode[classSymbol]);
        data.emplace_back(classDecl->classSourceRange, LSPTokenType::KEYWORD, std::vector<LSPTokenModifier>{});
        std::vector nameLspTokenModifiers = {LSPTokenModifier::DECLARATION};
        if (classSymbol->isAbstract) nameLspTokenModifiers.push_back(LSPTokenModifier::ABSTRACT);

        data.emplace_back(classDecl->nameSourceRange, LSPTokenType::CLASS, nameLspTokenModifiers);

        std::ranges::for_each(classSymbol->nestedClasses, [&data](const auto* nestedClassSymbol) {
           getSourceRanges(nestedClassSymbol, data);
        });

        std::ranges::for_each(classSymbol->methods, [&data](const auto* methodSymbol) {
           getSourceRanges(methodSymbol, data);
        });

        std::ranges::for_each(classSymbol->fields, [&data](const auto* fieldSymbol) {
           getSourceRanges(fieldSymbol, data);
        });

        std::ranges::for_each(classSymbol->genericArguments, [&data](const auto* typeParameterSymbol) {
            getSourceRanges(typeParameterSymbol, data);
        });

        std::ranges::for_each(classSymbol->superClasses, [&data](const auto* superClassType) {
           getSourceRanges(superClassType, data);
        });

        std::ranges::for_each(classDecl->modifiers, [&data](const auto* modifierNode) {
            getSourceRanges(modifierNode, data);
        });
    }

    void getSourceRanges(TypeParameterSymbol* typeParameterSymbol, std::vector<tokenData>& data) {
        // TODO - Need to store type refs too somehow
        // TODO - Probably make the map more general, to ASTNode or something
        auto typeRef = dynamic_cast<TypeRef*>(semanticAnalyser.symbolToASTNode[typeParameterSymbol]);

        data.emplace_back(typeRef->bodyRange, LSPTokenType::TYPE_PARAMETER, std::vector{LSPTokenModifier::DECLARATION});
    }

    void getSourceRanges(Type* type, std::vector<tokenData>& data) {
        auto typeRef = dynamic_cast<TypeRef*>(semanticAnalyser.symbolToASTNode[type]);

        if (auto classSymbol = dynamic_cast<ClassSymbol*>(type->typeSymbol)) {
            // Should only do that to the name and recursively visit children
            data.emplace_back(typeRef->bodyRange, getClassTokenType(classSymbol), std::vector<LSPTokenModifier>{});
        } else {
            data.emplace_back(typeRef->bodyRange, LSPTokenType::TYPE_PARAMETER, std::vector<LSPTokenModifier>{});
        }
    }

    void getSourceRanges(std::vector<ModifierNode*> modifierNodes, std::vector<tokenData>& data) {
        std::ranges::for_each(modifierNodes, [&data](const auto modifierNode) {
            data.emplace_back(modifierNode->sourceRange, LSPTokenType::MODIFIER, std::vector<LSPTokenModifier>{});
        });
    }

    void getSourceRanges(VariableSymbol* variableSymbol, std::vector<tokenData>& data) {
        auto variableDecl = dynamic_cast<FieldDecl*>(semanticAnalyser.symbolToASTNode[variableSymbol]);
        std::vector nameModifiers = {LSPTokenModifier::DECLARATION};
        if (variableSymbol->isStatic) nameModifiers.push_back(LSPTokenModifier::STATIC);

        if (variableSymbol->initExpr) getSourceRanges(variableSymbol->initExpr, data);

        getSourceRanges(variableSymbol->type, data);
        getSourceRanges(variableDecl->modifiers, data);

        if (auto fieldSymbol = dynamic_cast<FieldSymbol*>(variableSymbol)) {
            if (fieldSymbol->isAbstract) {
                nameModifiers.push_back(LSPTokenModifier::ABSTRACT);
            }
            data.emplace_back(variableDecl->nameSourceRange, LSPTokenType::PROPERTY, nameModifiers);
        } else {
            data.emplace_back(variableDecl->nameSourceRange, LSPTokenType::PROPERTY, nameModifiers);
        }
    }

    void getSourceRanges(FunctionSymbol* functionSymbol, std::vector<tokenData>& data) {
        auto functionDecl = dynamic_cast<MethodDecl*>(semanticAnalyser.symbolToASTNode[functionSymbol]);

        getSourceRanges(functionSymbol->returnType, data);
        std::ranges::for_each(functionSymbol->genericArguments, [&data](const auto* typeParameter) {
            getSourceRanges(typeParameter, data);
        });

        getSourceRanges(functionDecl->modifiers, data);

        // TODO - Add name source range to function symbol
        // Use functionSymbol->isStatic too

        std::ranges::for_each(functionSymbol->parameters, [&data](const auto* variableSymbol) {
            getSourceRanges(variableSymbol, data);
        });
    }

    void getSourceRanges(BoundExpr* expr, std::vector<tokenData>& data) {
        switch (expr->kind) {
            case BoundExprKind::EXPR:
                break;
            case BoundExprKind::TERNARY_EXPR:
                break;
            case BoundExprKind::FIELD_ACCESS_EXPR:
                break;
            case BoundExprKind::CALL_EXPR:
                break;
            case BoundExprKind::METHOD_CALL_EXPR:
                break;
            case BoundExprKind::VARIABLE_REF:
                break;
        }
    }
};



#endif //LANGUAGESERVER_H
