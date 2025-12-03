//
// Created by Agamjeet Singh on 03/12/25.
//

#include "../../include/parser/Parser.h"

#include "../../include/parser/Modifier.h"

enum class Modifier;
class FieldDecl;
class MethodDecl;
class ClassDecl;

KahwaFile *Parser::parseFile(const std::vector<Token> &tokens) const {
    return ParserWorker(tokens, astArena, diagnostic_engine).parseFile();
}

TypedefDecl *Parser::parseTypedef(const std::vector<Token> &tokens) const {
    return ParserWorker(tokens, astArena, diagnostic_engine).parseTypedef();
}

KahwaFile *Parser::ParserWorker::parseFile() {
    std::vector<TypedefDecl*> typedefDecls;
    while (idx < tokens.size()) {
        const Token& token = tokens[idx++];

        if (token.type == TokenType::TYPEDEF) {
            if (auto typedefDecl = parseTypedef()) {
                typedefDecls.push_back(typedefDecl);
            }
        } else {
            std::vector<Token> modifierTokens = next([](const Token& tok){ return !MODIFIER_TYPES.contains(tok.type); });
            idx += modifierTokens.size();

            std::vector<Modifier> modifiers;
            std::ranges::transform(modifierTokens, modifiers.begin(), [](const Token& token){ return tokenTypeToModifier(token.type); });

            if (next_is(TokenType::CLASS)) {
                // class-decl
                idx++;

                std::string name;
                std::vector<FieldDecl*> fields;
                std::vector<MethodDecl*> methods;
                std::vector<ClassDecl*> nestedClasses;

                SourceRange classSourceRange = next(1)[0].source_range;

                if (!next_is(TokenType::IDENTIFIER)) {
                    // TODO - Insert a bad node
                    diagnostic_engine.reportProblem(DiagnosticSeverity::ERROR, DiagnosticKind::EXPECTED_CLASS_NAME, SourceRange{next(1)[0].source_range}, toMsg(DiagnosticKind::EXPECTED_CLASS_NAME));
                    // Maybe jump to some safe point
                    continue;
                }

                name = *tokens[idx++].getIf<std::string>();

                if (!next_is(TokenType::LEFT_CURLY_BRACE)) {
                    // TODO - Insert a bad node
                    diagnostic_engine.reportProblem(DiagnosticSeverity::ERROR, DiagnosticKind::EXPECTED_SOMETHING, SourceRange{}, toMsg(DiagnosticKind::EXPECTED_SOMETHING, "{"));
                }

            } else {
                // variable-decl or function-decl
            }

            // TODO - Insert a bad node
            diagnostic_engine.reportProblem(DiagnosticSeverity::ERROR, DiagnosticKind::EXPECTED_DECLARATION, SourceRange{token.source_range}, toMsg(DiagnosticKind::EXPECTED_DECLARATION));
        }
    }

    return astArena.make<KahwaFile>(typedefDecls);
}

TypedefDecl *Parser::ParserWorker::parseTypedef() {
    // Assuming no generics
    if (next_is({TokenType::IDENTIFIER, TokenType::IDENTIFIER, TokenType::SEMI_COLON})) {

        auto* referredType = astArena.make<TypeRef>(*tokens[idx++].getIf<std::string>());
        return astArena.make<TypedefDecl>(*tokens[idx++].getIf<std::string>(), referredType);
    }
    return nullptr;
}

bool Parser::ParserWorker::next_is(TokenType expected) const {
    return next_is(std::vector{expected});
}

bool Parser::ParserWorker::next_is(const std::vector<TokenType> &expected) const {
    std::vector<Token> nextTokens = next(expected.size());
    std::vector<TokenType> nextTokenTypes;
    std::ranges::transform(nextTokens, nextTokenTypes.begin(), [](const Token& token){ return token.type; });
    return nextTokenTypes == expected;
}

std::vector<Token> Parser::ParserWorker::next(const std::function<bool(const Token &)> &until) const {
    return next(tokens.size(), until);
}


std::vector<Token> Parser::ParserWorker::next(std::size_t count, const std::function<bool(const Token &)> &until) const {
    std::vector<Token> res;
    std::size_t i = idx;
    while (i < tokens.size() && count-- > 0) {
        const Token& token = tokens[i++];
        if (!until(token)) {
            res.push_back(token);
        } else {
            break;
        }
    }

    return res;
}
