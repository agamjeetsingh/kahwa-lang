//
// Created by Agamjeet Singh on 29/11/25.
//

#include <gtest/gtest.h>
#include <magic_enum.hpp>
#include <random>

#include "../../include/source/SourceManager.h"
#include "../../include/tokeniser/Tokeniser.h"

class TokeniserTest : public testing::Test {
protected:
    DiagnosticEngine diagnostic_engine;
    Tokeniser tokeniser{diagnostic_engine};

    static std::vector<TokenType> toTokenType(std::vector<Token> tokens) {
        std::vector<TokenType> result{tokens.size()};
        std::ranges::transform(tokens, result.begin(), [](const Token& tok){ return tok.type; });

        return result;
    }

    std::vector<TokenType> singleLengthTokens = std::vector{
        TokenType::COLON, // ":"
        TokenType::SEMI_COLON, // ";"
        TokenType::COMMA, // ","
        TokenType::LEFT_CURLY_BRACE, // "{"
        TokenType::RIGHT_CURLY_BRACE, // "}"
        TokenType::LEFT_PAREN, // "("
        TokenType::RIGHT_PAREN, // ")"
        TokenType::LEFT_BRACKET, // "["
        TokenType::RIGHT_BRACKET, // "]"
        TokenType::EQUALS, // "="
        TokenType::LESS, // "<"
        TokenType::GREATER, // ">"
        TokenType::NOT, // "!"
        TokenType::PLUS, // "+"
        TokenType::MINUS, // "-"
        TokenType::STAR, // "*"
        TokenType::SLASH, // "/"
        TokenType::MODULO, // "%"
        TokenType::BITWISE_OR, // "|"
        TokenType::BITWISE_XOR, // "^"
        TokenType::QUESTION, // "?"
        TokenType::DOT, // "."
    };

    void expectTokenSequence(const std::string& input, const std::vector<TokenType>& expected) const {
        EXPECT_EQ(toTokenType(tokeniser.tokenise(0, input)), expected);
    }

    void testEqualsOperatorSequences(TokenType baseOpEquals) const {
        std::string baseOp = tokenTypeToString(baseOpEquals);
        baseOp = baseOp.substr(0, baseOp.size() - 1);
        
        expectTokenSequence(baseOp + "=", {baseOpEquals});
        expectTokenSequence(baseOp + "==", {baseOpEquals, TokenType::EQUALS});
        expectTokenSequence(baseOp + "===", {baseOpEquals, TokenType::DOUBLE_EQUALS});
    }

    void expectDiagnostics(const std::vector<Diagnostic>& diagnostics) const {
        EXPECT_EQ(diagnostic_engine.getAll(), diagnostics);
    }

    void expectNoDiagnostics() const {
        expectDiagnostics({});
    }
};

TEST_F(TokeniserTest, TokenisesSingleLengthTokensCorrectly) {
    for (auto tokType: singleLengthTokens) {
        expectTokenSequence(tokenTypeToString(tokType), {tokType});
    }

    expectNoDiagnostics();
}

TEST_F(TokeniserTest, TokenisesMultipleSingleLengthTokensCorrectly) {
    for (auto tokType1: singleLengthTokens) {
        for (auto tokType2: singleLengthTokens) {
            for (auto tokType3: singleLengthTokens) {
                for (auto tokType4: singleLengthTokens) {
                    std::string str = tokenTypeToString(tokType1) + " " + tokenTypeToString(tokType2) + "\t" + tokenTypeToString(tokType3) + "\n" + tokenTypeToString(tokType4);
                    expectTokenSequence(str, {tokType1, tokType2, tokType3, tokType4});
                }
            }
        }
    }

    expectNoDiagnostics();
}

TEST_F(TokeniserTest, TokenisesMultipleLengthTokensWithLongestMatching) {
    expectTokenSequence("==", {TokenType::DOUBLE_EQUALS});
    expectTokenSequence("===", {TokenType::DOUBLE_EQUALS, TokenType::EQUALS});
    expectTokenSequence("====", {TokenType::DOUBLE_EQUALS, TokenType::DOUBLE_EQUALS});

    expectTokenSequence("<=", {TokenType::LESS_EQUALS});
    expectTokenSequence("<<=", {TokenType::LEFT_SHIFT_EQUALS});
    expectTokenSequence("<<<=", {TokenType::LEFT_SHIFT, TokenType::LESS_EQUALS});
    expectTokenSequence("<==", {TokenType::LESS_EQUALS, TokenType::EQUALS});
    expectTokenSequence("<===", {TokenType::LESS_EQUALS, TokenType::DOUBLE_EQUALS});
    expectTokenSequence("<<==", {TokenType::LEFT_SHIFT_EQUALS, TokenType::EQUALS});
    expectTokenSequence("<<===", {TokenType::LEFT_SHIFT_EQUALS, TokenType::DOUBLE_EQUALS});

    expectTokenSequence(">=", {TokenType::GREATER_EQUALS});
    expectTokenSequence(">>=", {TokenType::RIGHT_SHIFT_EQUALS});
    expectTokenSequence(">>>=", {TokenType::RIGHT_SHIFT, TokenType::GREATER_EQUALS});
    expectTokenSequence(">==", {TokenType::GREATER_EQUALS, TokenType::EQUALS});
    expectTokenSequence(">===", {TokenType::GREATER_EQUALS, TokenType::DOUBLE_EQUALS});
    expectTokenSequence(">>==", {TokenType::RIGHT_SHIFT_EQUALS, TokenType::EQUALS});
    expectTokenSequence(">>===", {TokenType::RIGHT_SHIFT_EQUALS, TokenType::DOUBLE_EQUALS});

    expectTokenSequence("!=", {TokenType::NOT_EQUALS});
    expectTokenSequence("!==", {TokenType::NOT_EQUALS, TokenType::EQUALS});
    expectTokenSequence("!===", {TokenType::NOT_EQUALS, TokenType::DOUBLE_EQUALS});
    expectTokenSequence("!!===", {TokenType::NOT, TokenType::NOT_EQUALS, TokenType::DOUBLE_EQUALS});

    testEqualsOperatorSequences(TokenType::PLUS_EQUALS);
    testEqualsOperatorSequences(TokenType::MINUS_EQUALS);
    testEqualsOperatorSequences(TokenType::STAR_EQUALS);
    testEqualsOperatorSequences(TokenType::SLASH_EQUALS);
    testEqualsOperatorSequences(TokenType::MODULO_EQUALS);
    testEqualsOperatorSequences(TokenType::BITWISE_AND_EQUALS);
    testEqualsOperatorSequences(TokenType::BITWISE_OR_EQUALS);
    testEqualsOperatorSequences(TokenType::BITWISE_XOR_EQUALS);

    expectTokenSequence("++", {TokenType::INCREMENT});
    expectTokenSequence("+++", {TokenType::INCREMENT, TokenType::PLUS});

    expectTokenSequence("--", {TokenType::DECREMENT});
    expectTokenSequence("---", {TokenType::DECREMENT, TokenType::MINUS});

    expectTokenSequence("||", {TokenType::LOGICAL_OR});
    expectTokenSequence("|||", {TokenType::LOGICAL_OR, TokenType::BITWISE_OR});

    expectTokenSequence("&&", {TokenType::LOGICAL_AND});
    expectTokenSequence("&&&", {TokenType::LOGICAL_AND, TokenType::BITWISE_AND});

    expectNoDiagnostics();
}

TEST_F(TokeniserTest, TokeniserTokenisesIntegersCorrectly) {
    for (int i = -1000; i < 1000; i++) {
        if (i < 0) {
            expectTokenSequence(std::to_string(i), {TokenType::MINUS, TokenType::INTEGER});
        } else {
            expectTokenSequence(std::to_string(i), {TokenType::INTEGER});
        }

        auto tokens = tokeniser.tokenise(0, std::to_string(i));
        const Token& numTok = tokens.back();
        EXPECT_EQ (*numTok.getIf<int>(), abs(i));
    }

    expectNoDiagnostics();
}

TEST_F(TokeniserTest, TokeniserTokenisesFloatsCorrectly) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dist(-100.0, 100.0);

    for (int i = 0; i < 10000; i++) {
        const auto num = static_cast<float>(dist(gen));
        if (num < 0) {
            expectTokenSequence(std::to_string(num), {TokenType::MINUS, TokenType::FLOAT});
        } else {
            expectTokenSequence(std::to_string(num), {TokenType::FLOAT});
        }

        auto tokens = tokeniser.tokenise(0, std::to_string(num));
        const Token& numTok = tokens.back();
        EXPECT_TRUE((*numTok.getIf<float>() - abs(num)) < 1e-6f);
    }

    expectNoDiagnostics();
}

TEST_F(TokeniserTest, TokenisesStringsCorrectly) {
    for (int i = 0; i < 26; i++) {
        for (int j = 0; j < 26; j++) {
            for (int k = 0; k < 26; k++) {
                const char chars[] = {'\"', static_cast<char>(i + 'a'), static_cast<char>(j + 'a'), static_cast<char>(k + 'a'), '\"', '\0'};
                std::string str = chars;
                auto tokens = tokeniser.tokenise(0, str);

                EXPECT_EQ (tokens.size(), 1);
                EXPECT_EQ (tokens[0].type, TokenType::STRING_LITERAL);
                EXPECT_EQ (*tokens[0].getIf<std::string>(), str.substr(1, 3));
            }
        }
    }

    // Testing with spaces and other delimiters

    std::vector delimiters = {' ', '\t', '\r', '\n', '\f'};
    for (const char delimiter : delimiters) {
        std::string str = "\"abc123 " + std::to_string(delimiter) + " 123 Weird char Φ abc\"";
        auto tokens = tokeniser.tokenise(0, str);
        EXPECT_EQ(tokens.size(), 1);
        EXPECT_EQ(tokens[0].type, TokenType::STRING_LITERAL);
        EXPECT_EQ(*tokens[0].getIf<std::string>(), str.substr(1, str.length() - 2));
    }

    expectNoDiagnostics();
}


TEST_F(TokeniserTest, TokenisesMultipleTokenTypesCorrectly) {
    for (auto tokType1: magic_enum::enum_values<TokenType>()) {
        for (auto tokType2: magic_enum::enum_values<TokenType>()) {
            for (auto tokType3: magic_enum::enum_values<TokenType>()) {
                std::unordered_set skip = {TokenType::IDENTIFIER, TokenType::STRING_LITERAL, TokenType::CHAR_LITERAL, TokenType::INTEGER, TokenType::FLOAT, TokenType::BAD};
                if (skip.contains(tokType1) || skip.contains(tokType2) || skip.contains(tokType3)) continue;
                std::string str = tokenTypeToString(tokType1) + " \n" + tokenTypeToString(tokType2) + "\t\r" + tokenTypeToString(tokType3);
                expectTokenSequence(str, {tokType1, tokType2, tokType3});
            }
        }
    }

    expectNoDiagnostics();
}

TEST_F(TokeniserTest, ReportsDiagnosticForUnterminatedString) {
    const auto tokens = tokeniser.tokenise(0, "\" Unterminated string! Oh no! \n \t \r");
    EXPECT_TRUE (tokens.empty());

    expectDiagnostics({Diagnostic{DiagnosticSeverity::ERROR, DiagnosticKind::UNTERMINATED_STRING_LITERAL, SourceRange{0, 0}, toMsg(DiagnosticKind::UNTERMINATED_STRING_LITERAL)}});
}

TEST_F(TokeniserTest, ReportsDiagnosticForUnrecognisedToken) {
    const auto tokens = tokeniser.tokenise(0, "# Weird char");
    EXPECT_EQ (tokens.size(), 3);

    EXPECT_EQ (tokens[0].type, TokenType::BAD);
    EXPECT_EQ (*tokens[1].getIf<std::string>(), "Weird");
    EXPECT_EQ (*tokens[2].getIf<std::string>(), "char");

    expectDiagnostics({Diagnostic{DiagnosticSeverity::ERROR, DiagnosticKind::UNRECOGNISED_TOKEN, SourceRange{0, 0}, toMsg(DiagnosticKind::UNRECOGNISED_TOKEN)}});
}

TEST_F(TokeniserTest, TokeniserIdentifiesIdentifierCorrectly) {
    expectTokenSequence("abc123 234 abc _123 123_", {
        TokenType::IDENTIFIER,
        TokenType::INTEGER,
        TokenType::IDENTIFIER,
        TokenType::IDENTIFIER,
        TokenType::INTEGER, // 123
        TokenType::IDENTIFIER, // _
    });

    expectTokenSequence("class MyClass : SomeOtherClass {\n void foo() { } \n}", {
        TokenType::CLASS,
        TokenType::IDENTIFIER,
        TokenType::COLON,
        TokenType::IDENTIFIER,
        TokenType::LEFT_CURLY_BRACE,
        TokenType::IDENTIFIER,
        TokenType::IDENTIFIER,
        TokenType::LEFT_PAREN,
        TokenType::RIGHT_PAREN,
        TokenType::LEFT_CURLY_BRACE,
        TokenType::RIGHT_CURLY_BRACE,
        TokenType::RIGHT_CURLY_BRACE
    });

    expectNoDiagnostics();
}

TEST_F(TokeniserTest, TokeniserIdentifiesKeywordsSeparatelyFromIdentifiers) {
    std::string str;
    for (const auto keywordType: KEYWORD_TYPES) {
        str.append(keywordToString(keywordType));
        str.append(" ");
    }

    expectTokenSequence(str, {KEYWORD_TYPES.begin(), KEYWORD_TYPES.end()});

    expectTokenSequence("class _class for for-who for() if", {
        TokenType::CLASS,
        TokenType::IDENTIFIER,
        TokenType::FOR,
        TokenType::FOR,
        TokenType::MINUS, // -
        TokenType::IDENTIFIER, // who
        TokenType::FOR,
        TokenType::LEFT_PAREN,
        TokenType::RIGHT_PAREN,
        TokenType::IF,
    });

    expectNoDiagnostics();
}

TEST_F(TokeniserTest, TokeniserSkipsSingleLineComments) {
    expectTokenSequence("// This is a comment", {});

    expectTokenSequence("// This is a comment \n tokenise normally", {
        TokenType::IDENTIFIER,
        TokenType::IDENTIFIER
    });

    expectTokenSequence("// This is a comment \t This too is part of the comment \n"
    "123456 // Comment can begin after something too", {
        TokenType::INTEGER
    });

    expectTokenSequence("/ / This is not a comment", {
        TokenType::SLASH,
        TokenType::SLASH,
        TokenType::IDENTIFIER,
        TokenType::IDENTIFIER,
        TokenType::IDENTIFIER,
        TokenType::IDENTIFIER,
        TokenType::IDENTIFIER,
    });
}

TEST_F(TokeniserTest, TokeniserSkipsMultiLineCommentsCorrectly) {
    expectTokenSequence("/* This is a multi-line comment \n \n Part of comment \n */ tokenise normally", {
        TokenType::IDENTIFIER,
        TokenType::IDENTIFIER
    });

    expectTokenSequence("/*/ Doesn't close comment", {});

    // Unclosed multiline should also work
    expectTokenSequence("class A /* Multi-line comments can begin after // \n \n // \n /* 123.456", {
        TokenType::CLASS,
        TokenType::IDENTIFIER,
    });

    expectTokenSequence("/* Comment /* */ comment ended", {
        TokenType::IDENTIFIER,
        TokenType::IDENTIFIER
    });

    expectTokenSequence("/**/", {});
    expectTokenSequence("//**/ A single line comment", {});
}
