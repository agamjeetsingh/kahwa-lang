//
// Created by Agamjeet Singh on 03/12/25.
//

#ifndef PARSER_H
#define PARSER_H
#include "ClassDecl.h"
#include "KahwaFile.h"
#include "TypedefDecl.h"
#include "../tokeniser/Token.h"
#include "../arena/Arena.h"
#include "../diagnostics/DiagnosticEngine.h"

typedef std::function<bool(const Token&)> safePointFunc;

class Parser {
public:
    explicit Parser(Arena& astArena, DiagnosticEngine& diagnostic_engine): astArena(astArena), diagnostic_engine(diagnostic_engine) {
        ASTBuilder::setArena(&astArena);
    }

    [[nodiscard]] KahwaFile* parseFile(const std::vector<Token> &tokens) const;

    [[nodiscard]] TypedefDecl* parseTypedef(const std::vector<Token> &tokens) const;

    class ParserWorker {
    public:
        explicit ParserWorker(const std::vector<Token> &tokens, Arena& astArena, DiagnosticEngine& diagnostic_engine): tokens(tokens), astArena(astArena), diagnostic_engine(diagnostic_engine) {}

        std::vector<Modifier> getModifierList();

        KahwaFile* parseFile();

        TypedefDecl* parseTypedef();

        ClassDecl* parseClass(const safePointFunc& isSafePoint = isSafePointForFile);

        TypeRef* parseTypeRef(const safePointFunc& isSafePoint);

        MethodDecl* parseMethod();

        Block* parseBlock();

    private:
        std::vector<Token> tokens;
        std::size_t idx = 0;

        Arena& astArena;
        DiagnosticEngine& diagnostic_engine;

        [[nodiscard]] bool next_is(TokenType expected) const;

        [[nodiscard]] bool next_is(const std::vector<TokenType>& expected) const;

        std::vector<Token> next(const safePointFunc &until) const;

        std::vector<Token> next(std::size_t count, const safePointFunc &until = [](const Token& token){ return false; }) const;

        [[nodiscard]] Token next() const;

        void syncTo(const safePointFunc &isSafePoint);

        std::optional<Token> expect(TokenType tokenType, DiagnosticKind kind, const safePointFunc &isSafePoint, bool advance = true);

        std::optional<Token> expect(TokenType tokenType, const safePointFunc &isSafePoint, bool advance = true);

        std::optional<std::vector<Token>> expect(const std::vector<TokenType>& tokenTypes, const std::vector<DiagnosticKind>& kinds, const std::vector<safePointFunc>& isSafePoints, bool advance = true);

        std::optional<std::vector<Token>> expect(const std::vector<TokenType>& tokenTypes, const std::vector<safePointFunc>& isSafePoints, bool advance = true);

        [[nodiscard]] SourceRange getPrevTokSourceRange() const;

        static inline const safePointFunc isSafePointForFile = [](const Token& token) {
            return token.type == TokenType::IDENTIFIER || token.type == TokenType::TYPEDEF || MODIFIER_TYPES.contains(token.type);
        };

        static inline const safePointFunc isSafePointForClass = [](const Token& token) {
            return token.type == TokenType::IDENTIFIER || MODIFIER_TYPES.contains(token.type);
        };

        static inline const safePointFunc skipNothing = [](const Token& token) { return true; };

        void assertTokenSequence(const std::vector<TokenType> &expectedTypes) const;
    };

private:
    Arena& astArena;
    DiagnosticEngine& diagnostic_engine;
};



#endif //PARSER_H
