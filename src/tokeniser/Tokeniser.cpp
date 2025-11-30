//
// Created by Agamjeet Singh on 22/11/25.
//

#include "../../include/tokeniser/Tokeniser.h"

#include <cassert>

std::vector<Token> Tokeniser::TokeniserWorker::tokenise() {
    while (idx < str.length()) {
        const std::size_t curr_idx = idx;
        char c = str[idx++];
        if (DELIMITERS.contains(c)) continue;

        switch (c) {
            case ':' :
                tokens.emplace_back(TokenType::COLON, SourceRange{file_id, curr_idx});
                break;
            case ';' :
                tokens.emplace_back(TokenType::SEMI_COLON, SourceRange{file_id, curr_idx});
                break;
            case ',' :
                tokens.emplace_back(TokenType::COMMA, SourceRange{file_id, curr_idx});
                break;
            case '{' :
                tokens.emplace_back(TokenType::LEFT_CURLY_BRACE, SourceRange{file_id, curr_idx});
                break;
            case '}' :
                tokens.emplace_back(TokenType::RIGHT_CURLY_BRACE, SourceRange{file_id, curr_idx});
                break;
            case '(' :
                tokens.emplace_back(TokenType::LEFT_PAREN, SourceRange{file_id, curr_idx});
                break;
            case ')' :
                tokens.emplace_back(TokenType::RIGHT_PAREN, SourceRange{file_id, curr_idx});
                break;
            case '[' :
                tokens.emplace_back(TokenType::LEFT_BRACKET, SourceRange{file_id, curr_idx});
                break;
            case ']' :
                tokens.emplace_back(TokenType::RIGHT_BRACKET, SourceRange{file_id, curr_idx});
                break;
            case '=' :
                if (next_is("=")) {
                    tokens.emplace_back(TokenType::DOUBLE_EQUALS, SourceRange{file_id, curr_idx, 2});
                    idx++;
                } else {
                    tokens.emplace_back(TokenType::EQUALS, SourceRange{file_id, curr_idx});
                }
                break;
            case '<' :
                if (next_is("<=")) {
                    tokens.emplace_back(TokenType::LEFT_SHIFT_EQUALS, SourceRange{file_id, curr_idx, 3});
                    idx += 2;
                } else if (next_is("<")) {
                    tokens.emplace_back(TokenType::LEFT_SHIFT, SourceRange{file_id, curr_idx, 2});
                    idx++;
                } else if (next_is("=")) {
                    tokens.emplace_back(TokenType::LESS_EQUALS, SourceRange{file_id, curr_idx, 2});
                    idx++;
                } else {
                    tokens.emplace_back(TokenType::LESS, SourceRange{file_id, curr_idx});
                }
                break;
            case '>' :
                if (next_is(">=")) {
                    tokens.emplace_back(TokenType::RIGHT_SHIFT_EQUALS, SourceRange{file_id, curr_idx, 3});
                    idx += 2;
                } else if (next_is(">")) {
                    tokens.emplace_back(TokenType::RIGHT_SHIFT, SourceRange{file_id, curr_idx, 2});
                    idx++;
                } else if (next_is("=")) {
                    tokens.emplace_back(TokenType::GREATER_EQUALS, SourceRange{file_id, curr_idx, 2});
                    idx++;
                } else {
                    tokens.emplace_back(TokenType::GREATER, SourceRange{file_id, curr_idx});
                }
                break;
            case '!' :
                if (next_is("=")) {
                    tokens.emplace_back(TokenType::NOT_EQUALS, SourceRange{file_id, curr_idx, 2});
                    idx++;
                } else {
                    tokens.emplace_back(TokenType::NOT, SourceRange{file_id, curr_idx});
                }
                break;
            case '+' :
                if (next_is("+")) {
                    tokens.emplace_back(TokenType::INCREMENT, SourceRange{file_id, curr_idx, 2});
                    idx++;
                } else if (next_is("=")) {
                    tokens.emplace_back(TokenType::PLUS_EQUALS, SourceRange{file_id, curr_idx, 2});
                    idx++;
                } else {
                    tokens.emplace_back(TokenType::PLUS, SourceRange{file_id, curr_idx});
                }
                break;
            case '-' :
                if (next_is("-")) {
                    tokens.emplace_back(TokenType::DECREMENT, SourceRange{file_id, curr_idx, 2});
                    idx++;
                } else if (next_is("=")) {
                    tokens.emplace_back(TokenType::MINUS_EQUALS, SourceRange{file_id, curr_idx, 2});
                    idx++;
                } else {
                    tokens.emplace_back(TokenType::MINUS, SourceRange{file_id, curr_idx});
                }
                break;
            case '*' :
                if (next_is("=")) {
                    tokens.emplace_back(TokenType::STAR_EQUALS, SourceRange{file_id, curr_idx, 2});
                    idx++;
                } else {
                    tokens.emplace_back(TokenType::STAR, SourceRange{file_id, curr_idx});
                }
                break;
            case '/' :
                if (next_is("/")) {
                    idx++;
                    // `idx < str.length()` to avoid infinite looping
                    while (idx < str.length() && !next_is("\n")) { idx++; }
                } else if (next_is("*")) {
                    idx++;
                    // `idx + 1 < str.length()` to avoid infinite looping
                    while (idx + 1 < str.length()) {
                        if (next_is("*/")) {
                            idx += 2;
                            break;
                        }
                        idx++;
                    }
                } else if (next_is("=")) {
                    tokens.emplace_back(TokenType::SLASH_EQUALS, SourceRange{file_id, curr_idx, 2});
                    idx++;
                } else {
                    tokens.emplace_back(TokenType::SLASH, SourceRange{file_id, curr_idx});
                }
                break;
            case '%' :
                if (next_is("=")) {
                    tokens.emplace_back(TokenType::MODULO_EQUALS, SourceRange{file_id, curr_idx, 2});
                    idx++;
                } else {
                    tokens.emplace_back(TokenType::MODULO, SourceRange{file_id, curr_idx});
                }
                break;
            case '&' :
                if (next_is("&")) {
                    tokens.emplace_back(TokenType::LOGICAL_AND, SourceRange{file_id, curr_idx, 2});
                    idx++;
                } else if (next_is("=")) {
                    tokens.emplace_back(TokenType::BITWISE_AND_EQUALS, SourceRange{file_id, curr_idx, 2});
                    idx++;
                } else {
                    tokens.emplace_back(TokenType::BITWISE_AND, SourceRange{file_id, curr_idx});
                }
                break;
            case '|' :
                if (next_is("|")) {
                    tokens.emplace_back(TokenType::LOGICAL_OR, SourceRange{file_id, curr_idx, 2});
                    idx++;
                } else if (next_is("=")) {
                    tokens.emplace_back(TokenType::BITWISE_OR_EQUALS, SourceRange{file_id, curr_idx, 2});
                    idx++;
                } else {
                    tokens.emplace_back(TokenType::BITWISE_OR, SourceRange{file_id, curr_idx});
                }
                break;
            case '^' :
                if (next_is("=")) {
                    tokens.emplace_back(TokenType::BITWISE_XOR_EQUALS, SourceRange{file_id, curr_idx, 2});
                    idx++;
                } else {
                    tokens.emplace_back(TokenType::BITWISE_XOR, SourceRange{file_id, curr_idx});
                }
                break;
            case '?' :
                tokens.emplace_back(TokenType::QUESTION, SourceRange{file_id, curr_idx});
                break;
            case '.' :
                tokens.emplace_back(TokenType::DOT, SourceRange{file_id, curr_idx});
                break;
            case '\"': {
                if (auto maybeToken = tokeniseString(curr_idx)) {
                    tokens.push_back(maybeToken.value());
                } else {
                    diagnostic_engine.reportProblem(DiagnosticSeverity::ERROR, DiagnosticKind::UNTERMINATED_STRING_LITERAL, SourceLocation{file_id, curr_idx}, toMsg(DiagnosticKind::UNTERMINATED_STRING_LITERAL));
                    return tokens;
                }
                break;
            }
            default:
                if (std::isdigit(c)) {
                    idx--;
                    Token token1 = tokeniseNumber(curr_idx);
                    if (next_is(".")) {
                        idx++;
                        Token token2 = tokeniseNumber(curr_idx);
                        float num = *token1.getIf<int>();
                        float decimal = *token2.getIf<int>();
                        assert(decimal >= 0);
                        if (decimal > 0) {
                            int digits = std::to_string(decimal).size();
                            decimal /= std::pow(10.0f, digits);
                        }
                        tokens.emplace_back(TokenType::FLOAT, num + decimal, SourceRange{file_id, curr_idx, std::to_string(num + decimal).length()});
                    } else {
                        int num = *token1.getIf<int>();
                        tokens.emplace_back(TokenType::INTEGER, num, SourceRange{file_id, curr_idx, std::to_string(num).length()});
                    }
                } else if (std::isalpha(c) || c == '_') {
                    idx--;
                    std::string identifier_like = extractIdentifierLike();
                    if (TOKEN_MAP.contains(identifier_like)) {
                        tokens.emplace_back(TOKEN_MAP.at(identifier_like), SourceRange{file_id, curr_idx, identifier_like.length()});
                    } else {
                        tokens.emplace_back(TokenType::IDENTIFIER, identifier_like, SourceRange{file_id, curr_idx, identifier_like.length()});
                    }
                } else {
                    diagnostic_engine.reportProblem(DiagnosticSeverity::ERROR, DiagnosticKind::UNRECOGNISED_TOKEN, SourceRange{file_id, curr_idx}, toMsg(DiagnosticKind::UNRECOGNISED_TOKEN));
                }
        }
    }

    return tokens;
}


std::optional<Token> Tokeniser::TokeniserWorker::tokeniseString(std::size_t curr_idx) {
    std::string s;
    while (idx < str.length()) {
        char c = str[idx++];
        if (c == '\"') {
            return Token{TokenType::STRING_LITERAL, s, SourceRange{file_id, curr_idx, s.length() + 2}};
        }
        s += c;
    }

    return std::nullopt;
}

Token Tokeniser::TokeniserWorker::tokeniseNumber(std::size_t curr_idx) {
    std::string s = next([](char c) {
            return !isdigit(c);
        });
    idx += s.length();
    return Token{TokenType::INTEGER, std::stoi(s), SourceRange{file_id, curr_idx, s.length()}};
}

std::string Tokeniser::TokeniserWorker::extractIdentifierLike() {
    std::string s = std::string{str[idx++]};
    const std::string rest_of_s = next([](char c){ return !(isalnum(c) || c == '_'); });
    idx += rest_of_s.length();
    s += rest_of_s;
    return s;
}

bool Tokeniser::TokeniserWorker::next_is(const std::string &expected, const std::function<bool(char)> &until) const {
    const auto nextString = next(expected.length(), until);
    return nextString.size() == expected.length() && nextString == expected;
}

std::string Tokeniser::TokeniserWorker::next(const std::function<bool(char)> &until) const {
    return next(str.length(), until);
}

std::string Tokeniser::TokeniserWorker::next(std::size_t count, const std::function<bool(char)> &until) const {
    std::string res;
    std::size_t i = idx;
    while (i < str.length() && count-- > 0) {
        char c = str[i++];
        if (!until(c)) {
            res += c;
        } else {
            break;
        }
    }

    return res;
}

const std::unordered_set<char> Tokeniser::TokeniserWorker::DELIMITERS{' ', '\t', '\r', '\n', '\f'};
const std::unordered_map<std::string, TokenType> Tokeniser::TokeniserWorker::TOKEN_MAP{
            {"class", TokenType::CLASS},
            {"static", TokenType::STATIC},
            {"public", TokenType::PUBLIC},
            {"private", TokenType::PRIVATE},
            {"protected", TokenType::PROTECTED},
            {"open", TokenType::OPEN},
            {"final", TokenType::FINAL},
            {"abstract", TokenType::ABSTRACT},
            {"interface", TokenType::INTERFACE},
            {"typedef", TokenType::TYPEDEF},
            {"return", TokenType::RETURN},
            {"if", TokenType::IF},
            {"else", TokenType::ELSE},
            {"for", TokenType::FOR},
            {"while", TokenType::WHILE},
            {"break", TokenType::BREAK},
            {"continue", TokenType::CONTINUE},
            {"true", TokenType::TRUE},
            {"false", TokenType::FALSE},
            {"null", TokenType::NULL_LITERAL}
};
