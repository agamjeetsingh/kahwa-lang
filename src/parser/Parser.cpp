//
// Created by Agamjeet Singh on 03/12/25.
//

#include "../../include/parser/Parser.h"
#include "../../include/parser/Modifier.h"
#include "../../include/parser/expr/BinaryExpr.h"
#include "../../include/parser/expr/BinaryOp.h"
#include "../../include/parser/expr/CallExpr.h"
#include "../../include/parser/expr/IdentifierRef.h"
#include "../../include/parser/expr/UnaryExpr.h"
#include "../../include/parser/expr/literals/BoolLiteral.h"
#include "../../include/parser/expr/literals/FloatLiteral.h"
#include "../../include/parser/expr/literals/IntegerLiteral.h"
#include "../../include/parser/expr/literals/NullLiteral.h"
#include "../../include/parser/expr/literals/StringLiteral.h"
#include "../../include/parser/stmt/BreakStmt.h"
#include "../../include/parser/stmt/ContinueStmt.h"
#include "../../include/parser/stmt/ExprStmt.h"
#include "../../include/parser/stmt/ForLoop.h"
#include "../../include/parser/stmt/IfStmt.h"
#include "../../include/parser/stmt/ReturnStmt.h"
#include "../../include/parser/stmt/WhileLoop.h"
#include "../../include/symbols/TypeParameterSymbol.h"

KahwaFile *Parser::parseFile(const std::vector<Token> &tokens) const {
    return ParserWorker(tokens, astArena, diagnostic_engine).parseFile();
}

TypedefDecl *Parser::parseTypedef(const std::vector<Token> &tokens) const {
    return ParserWorker(tokens, astArena, diagnostic_engine).parseTypedef();
}

TypeRef* Parser::parseTypeRef(const std::vector<Token> &tokens) const {
    return ParserWorker(tokens, astArena, diagnostic_engine).parseTypeRef();
}

Expr *Parser::parseExpr(const std::vector<Token> &tokens) const {
    return ParserWorker(tokens, astArena, diagnostic_engine).parseExpr();
}

Stmt *Parser::parseStmt(const std::vector<Token> &tokens) const {
    return ParserWorker(tokens, astArena, diagnostic_engine).parseStmt();
}



KahwaFile *Parser::ParserWorker::parseFile() {
    auto kahwaFileBuilder = KahwaFileBuilder();

    while (idx < tokens.size()) {
        std::size_t save_idx = idx;

        getModifierList();

        const Token& token = tokens[idx++];

        if (token.type == TokenType::TYPEDEF) {
            idx = save_idx;
            if (auto typedefDecl = parseTypedef()) {
                kahwaFileBuilder.with(typedefDecl);
            } else {
                continue;
            }
        } else {
            if (token.type == TokenType::CLASS) {
                // class-decl
                idx = save_idx;
                if (ClassDecl *class_decl = parseClass()) {
                    kahwaFileBuilder.with(class_decl);
                }
            } else {
                diagnostic_engine.reportProblem(DiagnosticSeverity::ERROR, DiagnosticKind::EXPECTED_DECLARATION, token.source_range);

                continue; // TODO
                // variable-decl or function-decl

                idx = save_idx;

                // TODO - Distinguish between the two first

                // TODO - But with recovery being file-level and not class-level
                if (auto function = parseMethod()) {
                    kahwaFileBuilder.with(function);
                }

                continue;
            }

            // TODO - Insert a bad node
        }
    }

    return kahwaFileBuilder.build();
}

TypedefDecl *Parser::ParserWorker::parseTypedef() {
    // Assuming no generics

    const Token firstToken = tokens[idx];

    auto modifiers = getModifierList();

    idx++;

    if (auto typeRef = parseTypeRef(isSafePointForFile)) {
        if (auto nextTokens = expect({TokenType::IDENTIFIER, TokenType::SEMI_COLON}, {2, isSafePointForFile})) {
            return TypedefDeclBuilder(*nextTokens.value()[0].getIf<std::string>(), typeRef)
            .with(modifiers)
            .withBodyRange(SourceRange{firstToken, nextTokens->back()})
            .withTypedefSourceRange(typeRef->bodyRange)
            .withNameSourceRange(nextTokens.value()[1].source_range)
            .build();
        }
    }
    return nullptr;
}

TypeRef *Parser::ParserWorker::parseTypeRef(const safePointFunc &isSafePoint) {
    std::string name;
    if (auto nameToken = expect(TokenType::IDENTIFIER, isSafePoint)) {
        name = *nameToken.value().getIf<std::string>();
    } else {
        return nullptr;
    }
    auto typeRefBuilder = TypeRefBuilder(name);

    if (!next_is(TokenType::LESS)) return typeRefBuilder.build();

    // Generic
    idx++; // Skip '<'

    auto variance = Variance::INVARIANT;

    if (next_is(TokenType::OUT)) {
        variance = Variance::COVARIANT;
        idx++;
    } else if (next_is(TokenType::IN)) {
        variance = Variance::CONTRAVARIANT;
        idx++;
    }

    if (auto tok = expect(TokenType::IDENTIFIER, isSafePoint)) {
        idx--;
        if (auto arg = parseTypeRef(isSafePoint)) {
            typeRefBuilder.with(arg, variance);
        } else {
            return nullptr;
        }
    } else {
        return nullptr;
    }

    while (next_is(TokenType::COMMA)) {
        idx++; // Skip comma

        variance = Variance::INVARIANT;
        if (next_is(TokenType::OUT)) {
            variance = Variance::COVARIANT;
            idx++;
        } else if (next_is(TokenType::IN)) {
            variance = Variance::CONTRAVARIANT;
            idx++;
        }

        if (!expect(TokenType::IDENTIFIER, isSafePoint, false)) return nullptr;

        if (auto arg = parseTypeRef(isSafePoint)) {
            typeRefBuilder.with(arg, variance);
        } else {
            return nullptr;
        }
    }
    // TODO - Right shift equals needs to be handled too...
    if (next_is(TokenType::RIGHT_SHIFT)) {
        auto rightShiftToken = tokens[idx];
        std::vector<Token> newTokens;

        // Copy tokens before idx
        newTokens.reserve(tokens.size() + 1);
        for (int i = 0; i < idx; i++) {
            newTokens.emplace_back(tokens[i]);
        }

        newTokens.emplace_back(TokenType::GREATER, SourceRange{
            rightShiftToken.source_range.file_id,
            rightShiftToken.source_range.pos
        });
        newTokens.emplace_back(TokenType::GREATER, SourceRange{
            rightShiftToken.source_range.file_id,
            rightShiftToken.source_range.pos + 1,
        });

        for (int i = idx + 1; i < tokens.size(); i++) {
            newTokens.emplace_back(tokens[i]);
        }
        // Copy tokens after idx+1

        tokens = std::move(newTokens);
    }
    if (!expect(TokenType::GREATER, skipNothing)) return nullptr;

    return typeRefBuilder.build();
}

ClassDecl *Parser::ParserWorker::parseClass(const safePointFunc& isSafePoint) {
    std::vector<Modifier> modifiers = getModifierList();

    SourceRange classSourceRange = tokens[idx++].source_range;

    auto nameToken = expect(TokenType::IDENTIFIER, isSafePoint);
    if (!nameToken) {
        return nullptr;
    }

    auto classDeclBuilder = ClassDeclBuilder(*nameToken->getIf<std::string>());

    classDeclBuilder.withClassSourceRange(classSourceRange);
    classDeclBuilder.withNameSourceRange(nameToken->source_range);
    classDeclBuilder.with(modifiers);

    // Parse type parameters
    idx--; // So we can parse type parameters as a typeRef
    if (auto typeParametersAsTypeRef = parseTypeRef(isSafePoint)) {
        for (int i = 0; i < typeParametersAsTypeRef->args.size(); i++) {
            auto typeParameter = typeParametersAsTypeRef->args[i];
            auto variance = typeParametersAsTypeRef->variances[i];
            if (!typeParameter->args.empty()) {
                diagnostic_engine.reportProblem(
                    DiagnosticSeverity::ERROR,
                    DiagnosticKind::TYPE_PARAMETERS_CANNOT_HAVE_GENERIC_ARGUMENTS,
                    typeParameter->bodyRange,
                    toMsg(DiagnosticKind::TYPE_PARAMETERS_CANNOT_HAVE_GENERIC_ARGUMENTS, typeParameter->toString()));
                // So something like myClass<A, B<T>> would get treated as myClass<A, B>
                classDeclBuilder.withTypeParameter(TypeRefBuilder(typeParameter->identifier).build(), variance);
            } else {
                classDeclBuilder.withTypeParameter(typeParameter, variance);
            }
        }
    } else {
        return nullptr;
    }

    // Parse super classes
    if (next_is(TokenType::COLON)) {
        idx++;

        if (auto superClass = parseTypeRef(isSafePoint)) {
            classDeclBuilder.withSuperClass(superClass);
        } else {
            return nullptr;
        }

        while (next_is(TokenType::COMMA)) {
            idx++; // Skip comma

            if (auto superClass = parseTypeRef(isSafePoint)) {
                classDeclBuilder.withSuperClass(superClass);
            } else {
                return nullptr;
            }
        }
    }

    if (!expect(TokenType::LEFT_CURLY_BRACE, isSafePoint)) {
        return nullptr;
    }

    // Parse class body

    while (!next_is(TokenType::RIGHT_CURLY_BRACE)) {
        // Process class-member

        std::size_t save_idx = idx;
        getModifierList();
        if (next_is(TokenType::CLASS)) {
            idx = save_idx;

            if (auto classDecl = parseClass()) {
                classDeclBuilder.with(classDecl);
            } else {
                continue;
            }
        } else if (auto typeRef = parseTypeRef(isSafePointForClass)) {
            // Could be either <type of variable> or <return type of method> or <name of constructor>

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
                    if (auto methodDecl = parseMethod(isSafePoint)) {
                        classDeclBuilder.with(methodDecl);
                    } else {
                        continue;
                    }

                    // TODO
                    continue;
                }
                // field OR error TODO

                idx = save_idx;

                // TODO
            }
        }
    }

    expect(TokenType::RIGHT_CURLY_BRACE, isSafePoint);

    const std::size_t file_id = nameToken->source_range.file_id;
    const std::size_t length = 1; // TODO

    return classDeclBuilder.withBodyRange({file_id, classSourceRange.pos, length}).build();
}

MethodDecl *Parser::ParserWorker::parseMethod(const safePointFunc& isSafePoint) {
    const std::vector<Modifier>& modifiers = getModifierList();

    auto returnType = parseTypeRef(isSafePoint);

    assertTokenSequence({TokenType::IDENTIFIER, TokenType::LEFT_PAREN});

    auto nameToken = tokens[idx++];
    auto methodDeclBuilder = MethodDeclBuilder(*nameToken.getIf<std::string>(), returnType, nullptr);
    methodDeclBuilder.with(modifiers);
    methodDeclBuilder.withNameSourceRange(nameToken.source_range);
    methodDeclBuilder.withReturnTypeSourceRange(returnType->bodyRange);

    idx++; // Skipping opening parenthesis '('

    if (!next_is(TokenType::RIGHT_PAREN)) {
        if (auto paramTypeRef = parseTypeRef(isSafePoint)) {
            if (auto paramNameToken = expect(TokenType::IDENTIFIER, isSafePoint)) {
                methodDeclBuilder.with({paramTypeRef, *paramNameToken.value().getIf<std::string>()});
            } else {
                return nullptr;
            }
        } else {
            return nullptr;
        }
    }

    while (next_is(TokenType::COMMA)) {
        idx++; // Skip comma

        if (auto paramTypeRef = parseTypeRef(isSafePoint)) {
            if (auto paramNameToken = expect(TokenType::IDENTIFIER, isSafePoint)) {
                methodDeclBuilder.with({paramTypeRef, *paramNameToken.value().getIf<std::string>()});
            } else {
                return nullptr;
            }
        } else {
            return nullptr;
        }
    }

    if (!expect(TokenType::RIGHT_PAREN, isSafePoint)) {
        return nullptr;
    }

    Block* block;

    if (expect(TokenType::LEFT_CURLY_BRACE, isSafePoint)) {
        idx--;
        block = parseBlock();
        if (!block) {
            return nullptr;
        }
    } else {
        return nullptr;
    }

    SourceRange bodyRange{1, 1}; // TODO
    methodDeclBuilder.withBodyRange(bodyRange);
    methodDeclBuilder.with(block);

    return methodDeclBuilder.build();
}

Block *Parser::ParserWorker::parseBlock(const safePointFunc& isSafePoint) {
    idx++; // Move past '{'

    auto blockBuilder = BlockBuilder();

    while (!next_is(TokenType::RIGHT_CURLY_BRACE)) {
        if (auto* stmt = parseStmt(isSafePoint)) {
            blockBuilder.with(stmt);
        } else {
            return nullptr; // TODO, maybe don't discard the whole block
        }
    }

    idx++; // Move past '}'

    return blockBuilder.build();
}

Stmt *Parser::ParserWorker::parseStmt(const safePointFunc &isSafePoint) {
    switch (const auto& nextTok = tokens[idx].type) {
        case TokenType::BREAK: {
            // Skip past 'break'
            auto breakStmt = astArena.make<BreakStmt>(tokens[idx++].source_range);
            if (!expect(TokenType::SEMI_COLON, skipNothing, false)) {
                return nullptr;
            }
            idx++; // Skip past ';'
            return breakStmt;
        }
        case TokenType::CONTINUE: {
            auto continueStmt = astArena.make<ContinueStmt>(tokens[idx++].source_range);
            if (!expect(TokenType::SEMI_COLON, skipNothing, false)) {
                return nullptr;
            }
            idx++; // Skip past ';'
            return continueStmt;
        }
        case TokenType::FOR: {
            SourceRange forRange = tokens[idx++].source_range;
            expect(TokenType::LEFT_PAREN, skipNothing);
            Stmt *init;
            if (init = parseStmt(isSafePoint); !init) return nullptr;
            Expr *cond;
            if (cond = parseExpr(isSafePoint); !cond) return nullptr;
            Stmt *step;
            if (step = parseStmt(isSafePoint); !step) return nullptr;
            SourceRange bodyRange = SourceRange{0, 0}; // TODO
            Block *body;
            if (body = parseBlock(isSafePoint); !body) return nullptr;
            return astArena.make<ForLoop>(init, cond, step, body, bodyRange, forRange);
        }
        case TokenType::IF: {
            SourceRange ifRange = tokens[idx++].source_range;
            expect(TokenType::LEFT_PAREN, skipNothing);
            Expr* cond;
            if (cond = parseExpr(isSafePoint); !cond) return nullptr;
            expect(TokenType::RIGHT_PAREN, skipNothing);
            Block* ifBlock;
            if (ifBlock = parseBlock(isSafePoint); !ifBlock) return nullptr;
            Block* elseBlock = nullptr;
            if (next_is(TokenType::ELSE)) {
                idx++; // Skip past the 'else'
                if (elseBlock = parseBlock(isSafePoint); elseBlock) return nullptr;
            }
            SourceRange bodyRange = SourceRange{0, 0}; // TODO

            return astArena.make<IfStmt>(cond, ifBlock, (elseBlock == nullptr || elseBlock->stmts.empty()) ? nullptr : elseBlock, bodyRange, ifRange);
        }
        case TokenType::RETURN: {
            SourceRange returnRange = tokens[idx++].source_range;
            Expr* expr;
            if (expr = parseExpr(isSafePoint); !expr) return nullptr;
            SourceRange bodyRange = SourceRange{0, 0}; // TODO

            idx++; // Skip past ';'

            return astArena.make<ReturnStmt>(expr, bodyRange, returnRange);
        }
        case TokenType::WHILE: {
            SourceRange whileRange = tokens[idx++].source_range;
            Expr* cond;
            if (cond = parseExpr(isSafePoint); !cond) return nullptr;
            Block* body;
            if (body = parseBlock(isSafePoint); !body) return nullptr;
            SourceRange bodyRange = SourceRange{0, 0}; // TODO

            return astArena.make<WhileLoop>(cond, body, bodyRange, whileRange);
        }
        default: {
            Expr* expr = parseExpr(isSafePoint);
            if (!expr) return nullptr;

            idx++; // Skip past ';'

            return astArena.make<ExprStmt>(expr, expr->bodyRange);
        }
    }
}

Expr *Parser::ParserWorker::parseExpr(const safePointFunc &isSafePoint, int min_bp) {
    // Handling atoms, brackets '(' and prefix operators
    auto lhs = [this, isSafePoint]()->Expr* {
        switch (tokens[idx].type) {
            case TokenType::TRUE:
                return astArena.make<BoolLiteral>(true, tokens[idx++].source_range);

            case TokenType::FALSE:
                return astArena.make<BoolLiteral>(false, tokens[idx++].source_range);

            case TokenType::FLOAT:
                return astArena.make<FloatLiteral>(
                    *tokens[idx].getIf<float>(),
                    tokens[idx++].source_range
                );

            case TokenType::INTEGER:
                return astArena.make<IntegerLiteral>(
                    *tokens[idx].getIf<int>(),
                    tokens[idx++].source_range
                );

            case TokenType::NULL_LITERAL:
                return astArena.make<NullLiteral>(tokens[idx++].source_range);

            case TokenType::STRING_LITERAL:
                return astArena.make<StringLiteral>(
                    *tokens[idx].getIf<std::string>(),
                    tokens[idx++].source_range
                );

            case TokenType::LEFT_PAREN: {
                idx++;
                auto inner_lhs = parseExpr(isSafePoint, 0);
                if (expect(TokenType::RIGHT_PAREN, skipNothing, false)) idx++;
                return inner_lhs;
            }

            case TokenType::IDENTIFIER: {
                return astArena.make<IdentifierRef>(*tokens[idx].getIf<std::string>(), tokens[idx++].source_range);
            }

            case TokenType::PLUS:
            case TokenType::MINUS:
            case TokenType::NOT:
            case TokenType::INCREMENT:
            case TokenType::DECREMENT: {
                auto tokType = tokens[idx].type;
                auto r_bp = prefixBindingPower(tokType).value();
                idx++;

                SourceRange bodyRange = SourceRange{0, 0}; // TODO

                auto rhs = parseExpr(isSafePoint, r_bp);
                return astArena.make<UnaryExpr>(rhs, tokenTypeToPrefixUnaryOp(tokType), bodyRange);
            }

            default: ;
        }
    }();

    while (!next(1).empty()) {
        auto tok = tokens[idx];

        if (auto bp = postfixBindingPower(tok.type)) {
            auto l_bp = bp.value();
            if (l_bp < min_bp) break;

            idx++;

            SourceRange bodyRange = SourceRange{0, 0}; // TODO

            if (tok.type == TokenType::LEFT_BRACKET) {
                // indexing with '['

                auto rhs = parseExpr(isSafePoint, 0);
                // TODO - Actually should be a call-expression like new struct
                lhs = astArena.make<BinaryExpr>(lhs, rhs, tokenTypeToBinaryOp(tok.type).value(), bodyRange);
            } else if (tok.type == TokenType::LEFT_PAREN) {
                // call expressions with '('

                std::vector<Expr*> args;

                if (!next_is(TokenType::RIGHT_PAREN)) {
                    args.push_back(parseExpr(isSafePoint, 0));
                }

                while (next_is(TokenType::COMMA)) {
                    idx++; // Skip the comma
                    args.push_back(parseExpr(isSafePoint, 0));
                }

                if (expect(TokenType::RIGHT_PAREN, skipNothing, false)) idx++;

                lhs = astArena.make<CallExpr>(lhs, args, bodyRange);
            } else {
                lhs = astArena.make<UnaryExpr>(lhs, tokenTypeToPostfixUnaryOp(tok.type), bodyRange);
            }
            continue;
        }

        if (auto bp = infixBindingPower(tok.type)) {
            auto [l_bp, r_bp] = bp.value();
            if (l_bp < min_bp) break;

            idx++;

            auto rhs = parseExpr(isSafePoint, r_bp);
            if (!rhs) return nullptr;

            SourceRange bodyRange = SourceRange{0, 0}; // TODO

            lhs = astArena.make<BinaryExpr>(lhs, rhs, tokenTypeToBinaryOp(tok.type).value(), bodyRange);
            continue;
        }

        break;
    }

    return lhs;
}


void Parser::ParserWorker::assertTokenSequence(const std::vector<TokenType> &expectedTypes) const {
    std::vector<TokenType> actualTypes{expectedTypes.size()};
    std::vector<Token> actualTokens = next(expectedTypes.size());
    std::ranges::transform(actualTokens, actualTypes.begin(), [](const Token& token){ return token.type; });

    assert(actualTypes == expectedTypes);
}

bool Parser::ParserWorker::next_is(TokenType expected) const {
    return next_is(std::vector{expected});
}

bool Parser::ParserWorker::next_is(const std::vector<TokenType> &expected) const {
    std::vector<Token> nextTokens = next(expected.size());
    std::vector<TokenType> nextTokenTypes{nextTokens.size()};
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

void Parser::ParserWorker::syncTo(const safePointFunc &isSafePoint) {
    idx += next([&isSafePoint](const Token& token) { return isSafePoint(token); }).size();
}

std::optional<Token> Parser::ParserWorker::expect(const TokenType tokenType, const DiagnosticKind kind, const safePointFunc &isSafePoint, bool advance) {
    const auto nextToken = next(1);
    if (nextToken.size() == 1 && nextToken[0].type == tokenType) {
        return advance ? tokens[idx++] : tokens[idx];
    }

    diagnostic_engine.reportProblem(DiagnosticSeverity::ERROR, kind, getPrevTokSourceRange());
    syncTo(isSafePoint);
    return std::nullopt;
}

std::optional<Token> Parser::ParserWorker::expect(TokenType tokenType, const std::function<bool(const Token &)> &isSafePoint, bool advance) {
    return expect(tokenType, expectedTokenTypeToDiagnosticKind(tokenType), isSafePoint, advance);
}

std::optional<std::vector<Token>> Parser::ParserWorker::expect(const std::vector<TokenType>& tokenTypes, const std::vector<DiagnosticKind>& kinds, const std::vector<safePointFunc>& isSafePoints, bool advance) {
    assert(tokenTypes.size() == kinds.size() && kinds.size() == isSafePoints.size());
    std::vector<Token> res;
    const std::size_t curr_idx = idx;
    for (int i = 0; i < tokenTypes.size(); i++) {
        if (curr_idx + i < tokens.size()) {
            if (const auto nextToken = tokens[curr_idx + i]; nextToken.type == tokenTypes[i]) {
                res.emplace_back(nextToken);
            } else {
                diagnostic_engine.reportProblem(DiagnosticSeverity::ERROR, kinds[i], getPrevTokSourceRange());
                syncTo(isSafePoints[i]);
                return std::nullopt;
            }
        } else {
            diagnostic_engine.reportProblem(DiagnosticSeverity::ERROR, kinds[i], getPrevTokSourceRange());
            syncTo(isSafePoints[i]);
            return std::nullopt;
        }

        if (advance) idx++;
    }
    return res;
}

std::optional<std::vector<Token>> Parser::ParserWorker::expect(const std::vector<TokenType> &tokenTypes, const std::vector<std::function<bool(const Token &)>>& isSafePoints, bool advance) {
    std::vector<DiagnosticKind> kinds{tokenTypes.size()};
    std::ranges::transform(tokenTypes, kinds.begin(), [](const TokenType type){ return expectedTokenTypeToDiagnosticKind(type); });
    return expect(tokenTypes, kinds, isSafePoints, advance);
}

SourceRange Parser::ParserWorker::getPrevTokSourceRange() const {
    return idx == 0 ? SourceRange{tokens.empty() ? -1 : tokens[0].source_range.file_id, 0} : tokens[idx - 1].source_range;
}

std::vector<Modifier> Parser::ParserWorker::getModifierList() {
    std::vector<Token> modifierTokens = next([](const Token& tok){ return !MODIFIER_TYPES.contains(tok.type); });
    idx += modifierTokens.size();

    std::vector<Modifier> modifiers{modifierTokens.size()};
    std::ranges::transform(modifierTokens, modifiers.begin(), [](const Token& token){ return tokenTypeToModifier(token.type); });

    return modifiers;
}

std::optional<int> Parser::ParserWorker::prefixBindingPower(TokenType tokenType) {
    switch (tokenType) {
        case TokenType::PLUS:
        case TokenType::MINUS:
        case TokenType::NOT:
        case TokenType::INCREMENT:
        case TokenType::DECREMENT:
            return 9;
        default:
            return std::nullopt;
    }
}

std::optional<int> Parser::ParserWorker::postfixBindingPower(TokenType tokenType) {
    switch (tokenType) {
        case TokenType::INCREMENT:
        case TokenType::DECREMENT:
        case TokenType::LEFT_PAREN: // '(' for call expressions
        case TokenType::LEFT_BRACKET: // '[' for indexing
            return 11;
        default:
            return std::nullopt;
    }
}


std::optional<std::pair<int, int>> Parser::ParserWorker::infixBindingPower(TokenType tokenType) {
    switch (tokenType) {
        case TokenType::EQUALS:
            return std::pair{2, 1};
        case TokenType::PLUS:
        case TokenType::MINUS:
            return std::pair{5, 6};
        case TokenType::STAR:
        case TokenType::SLASH:
            return std::pair{7, 8};
        case TokenType::DOT:
            return std::pair{14, 13};
        default:
            return std::nullopt;
    }
}

