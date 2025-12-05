//
// Created by Agamjeet Singh on 03/12/25.
//

#include "../../include/parser/Parser.h"

#include "../../include/parser/Modifier.h"

KahwaFile *Parser::parseFile(const std::vector<Token> &tokens) const {
    return ParserWorker(tokens, astArena, diagnostic_engine).parseFile();
}

TypedefDecl *Parser::parseTypedef(const std::vector<Token> &tokens) const {
    return ParserWorker(tokens, astArena, diagnostic_engine).parseTypedef();
}

KahwaFile *Parser::ParserWorker::parseFile() {
    std::vector<TypedefDecl*> typedefDecls;
    std::vector<ClassDecl*> classDecls;
    std::vector<MethodDecl*> functionDecls;
    std::vector<FieldDecl*> variableDecls;

    while (idx < tokens.size()) {
        std::size_t save_idx = idx;
        const Token& token = tokens[idx++];

        getModifierList();

        if (token.type == TokenType::TYPEDEF) {
            idx = save_idx;
            if (auto typedefDecl = parseTypedef()) {
                typedefDecls.push_back(typedefDecl);
            } else {
                continue;
            }
        } else {
            if (next_is(TokenType::CLASS)) {
                // class-decl
                idx = save_idx;
                if (ClassDecl *class_decl = parseClass()) {
                    classDecls.push_back(class_decl);
                }
            } else {
                continue; // TODO
                // variable-decl or function-decl

                idx = save_idx;

                // TODO - Distinguish between the two first

                // TODO - But with recovery being file-level and not class-level
                if (auto function = parseMethod()) {
                    functionDecls.push_back(function);
                }

                continue;
            }

            // TODO - Insert a bad node
            diagnostic_engine.reportProblem(DiagnosticSeverity::ERROR, DiagnosticKind::EXPECTED_DECLARATION, SourceRange{token.source_range}, toMsg(DiagnosticKind::EXPECTED_DECLARATION));
        }
    }

    return astArena.make<KahwaFile>(typedefDecls, classDecls, functionDecls, variableDecls);
}

TypedefDecl *Parser::ParserWorker::parseTypedef() {
    // idx is pointing to token right after "typedef"
    // Assuming no generics

    SourceRange firstTokenSourceRange = tokens[idx].source_range;

    auto modifiers = getModifierList();

    if (const auto nextTokens = expect(
        std::vector{TokenType::TYPEDEF, TokenType::IDENTIFIER, TokenType::IDENTIFIER, TokenType::SEMI_COLON},
        std::vector(4, isSafePointForFile)
        )) {
        auto* referredType = astArena.make<TypeRef>(*nextTokens.value()[1].getIf<std::string>());
        return astArena.make<TypedefDecl>(
            *nextTokens.value()[2].getIf<std::string>(),
            modifiers,
            referredType,
            nextTokens.value()[0].source_range,
            nextTokens.value()[1].source_range,
            SourceRange{firstTokenSourceRange, nextTokens->back().source_range});
    }
    return nullptr;
}

ClassDecl *Parser::ParserWorker::parseClass() {
    std::vector<Modifier> modifiers = getModifierList();
    std::vector<TypeRef*> superClasses;
    std::vector<FieldDecl*> fields;
    std::vector<MethodDecl*> methods;
    std::vector<ClassDecl*> nestedClasses;

    SourceRange classSourceRange = tokens[idx++].source_range;

    auto nameToken = expect(TokenType::IDENTIFIER, isSafePointForFile);
    if (!nameToken) {
        return nullptr;
    }

    std::string name = *nameToken->getIf<std::string>();
    SourceRange nameSourceRange = nameToken->source_range;

    // TODO - Parse optional super classes

    if (!expect(TokenType::LEFT_CURLY_BRACE, isSafePointForFile)) {
        return nullptr;
    }

    // Parse class body

    while (!next_is(TokenType::RIGHT_CURLY_BRACE)) {
        // Process class-member

        std::size_t save_idx = idx;
        getModifierList();

        if (auto nextToken1 = expect(TokenType::IDENTIFIER, isSafePointForClass)) {
            // Could be type of variable or return type of method or name of constructor

            if (next_is(TokenType::LEFT_PAREN)) {
                // constructor

                idx = save_idx;

                // TODO
                continue;
            }

            if (auto nextToken2 = expect(TokenType::IDENTIFIER, isSafePointForClass)) {

                // left parenthesis -> method, otherwise field declaration
                if (next_is(TokenType::LEFT_PAREN)) {
                    // method

                    idx = save_idx;

                    // TODO
                    continue;
                }
                // field OR error TODO

                idx = save_idx;

                // TODO
            }
        }
    }

    expect(TokenType::RIGHT_CURLY_BRACE, isSafePointForFile);

    const std::size_t file_id = nameToken->source_range.file_id;
    const std::size_t length = 1; // TODO

    SourceRange bodyRange{file_id, classSourceRange.pos, length};

    return astArena.make<ClassDecl>(name, classSourceRange, nameSourceRange, bodyRange, modifiers, superClasses, fields, methods, nestedClasses);
}

MethodDecl *Parser::ParserWorker::parseMethod() {
    const std::vector<Modifier>& modifiers = getModifierList();
    assertTokenSequence(3, {TokenType::IDENTIFIER, TokenType::IDENTIFIER, TokenType::LEFT_PAREN});

    std::vector<std::pair<TypeRef*, std::string>> parameters;
    Block* block;

    std::vector<Token> nextTokens = next(3);

    auto returnType = astArena.make<TypeRef>(*nextTokens[0].getIf<std::string>());
    auto returnTypeSourceRange = nextTokens[0].source_range;
    std::string name = *nextTokens[1].getIf<std::string>();
    auto nameSourceRange = nextTokens[1].source_range;
    idx += 3;

    while (next_is(TokenType::RIGHT_PAREN)) {
        auto tokens = expect(
            {TokenType::IDENTIFIER, TokenType::IDENTIFIER},
            std::vector(3, isSafePointForClass));
        if (!tokens) {
            return nullptr;
        }

        auto paramType = astArena.make<TypeRef>(*tokens.value()[0].getIf<std::string>());
        const std::string& paramName = *tokens.value()[1].getIf<std::string>();

        parameters.emplace_back(paramType, paramName);
    }

    if (expect(TokenType::LEFT_CURLY_BRACE, isSafePointForClass)) {
        idx--;
        block = parseBlock();
        if (!block) {
            return nullptr;
        }
    } else {
        return nullptr;
    }

    SourceRange bodyRange{1, 1}; // TODO

    return astArena.make<MethodDecl>(name, modifiers, returnType, parameters, block, returnTypeSourceRange, nameSourceRange, bodyRange);
}

Block *Parser::ParserWorker::parseBlock() {

}


void Parser::ParserWorker::assertTokenSequence(const std::size_t count, const std::vector<TokenType> &expectedTypes) const {
    std::vector<TokenType> actualTypes;
    std::vector<Token> actualTokens = next(count);
    std::ranges::transform(actualTokens, actualTypes.begin(), [](const Token& token){ return token.type; });

    assert(actualTypes == expectedTypes);
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

Token Parser::ParserWorker::next() const {
    return next(1)[0];
}

void Parser::ParserWorker::syncTo(const std::function<bool(const Token&)> &isSafePoint) {
    idx += next([&isSafePoint](const Token& token) { return isSafePoint(token); }).size();
}

std::optional<Token> Parser::ParserWorker::expect(const TokenType tokenType, const DiagnosticKind kind, const std::function<bool(const Token&)> &isSafePoint) {
    const auto nextToken = next(1);
    if (nextToken.size() == 1) {
        if (nextToken[0].type == tokenType) {
            return tokens[idx++];
        }
    }

    diagnostic_engine.reportProblem(DiagnosticSeverity::ERROR, kind, getPrevTokSourceRange(), toMsg(kind));
    syncTo(isSafePoint);
    return std::nullopt;
}

std::optional<Token> Parser::ParserWorker::expect(TokenType tokenType, const std::function<bool(const Token &)> &isSafePoint) {
    return expect(tokenType, expectedTokenTypeToDiagnosticKind(tokenType), isSafePoint);
}


std::optional<std::vector<Token>> Parser::ParserWorker::expect(const std::vector<TokenType>& tokenTypes, const std::vector<DiagnosticKind>& kinds, const std::vector<std::function<bool(const Token&)>>& isSafePoints) {
    assert(tokenTypes.size() == kinds.size() && kinds.size() == isSafePoints.size());
    std::vector<Token> res;
    for (int i = 0; i < tokenTypes.size(); i++) {
        if (auto tok = expect(tokenTypes[i], kinds[i], isSafePoints[i])) {
            res.emplace_back(tok.value());
        } else {
            return std::nullopt;
        }
    }
    return res;
}

std::optional<std::vector<Token>> Parser::ParserWorker::expect(const std::vector<TokenType> &tokenTypes, const std::vector<std::function<bool(const Token &)> > &isSafePoints) {
    std::vector<DiagnosticKind> kinds{tokenTypes.size()};
    std::ranges::transform(tokenTypes, kinds.begin(), [](const TokenType type){ return expectedTokenTypeToDiagnosticKind(type); });
    return expect(tokenTypes, kinds, isSafePoints);
}

SourceRange Parser::ParserWorker::getPrevTokSourceRange() const {
    return idx == 0 ? SourceRange{tokens.empty() ? -1 : tokens[0].source_range.file_id, 0} : tokens[idx - 1].source_range;
}

std::vector<Modifier> Parser::ParserWorker::getModifierList() {
    std::vector<Token> modifierTokens = next([](const Token& tok){ return !MODIFIER_TYPES.contains(tok.type); });
    idx += modifierTokens.size();

    std::vector<Modifier> modifiers;
    std::ranges::transform(modifierTokens, modifiers.begin(), [](const Token& token){ return tokenTypeToModifier(token.type); });

    return modifiers;
}

