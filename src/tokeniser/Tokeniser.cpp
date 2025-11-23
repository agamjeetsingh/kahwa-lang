//
// Created by Agamjeet Singh on 22/11/25.
//

#include "../../include/tokeniser/Tokeniser.h"

std::optional<std::vector<Token> > Tokeniser::tokenise() {
    while (idx < str.length()) {
        char c = str[idx++];
        if (delimiters.contains(c)) continue;

        switch (c) {
            case ':' :
                tokens.emplace_back(TokenType::COLON);
                break;
            case ';' :
                tokens.emplace_back(TokenType::SEMI_COLON);
                break;
            case ',' :
                tokens.emplace_back(TokenType::COMMA);
                break;
            case '{' :
                tokens.emplace_back(TokenType::LEFT_CURLY_BRACE);
                break;
            case '}' :
                tokens.emplace_back(TokenType::RIGHT_CURLY_BRACE);
                break;
            case '(' :
                tokens.emplace_back(TokenType::LEFT_PAREN);
                break;
            case ')' :
                tokens.emplace_back(TokenType::RIGHT_PAREN);
                break;
            case '[' :
                tokens.emplace_back(TokenType::LEFT_BRACKET);
                break;
            case ']' :
                tokens.emplace_back(TokenType::RIGHT_BRACKET);
                break;
            case '=' :
                if (next_is("=")) {
                    tokens.emplace_back(TokenType::DOUBLE_EQUALS);
                    idx++;
                } else {
                    tokens.emplace_back(TokenType::EQUALS);
                }
                break;
            case '<' :
                if (next_is("<=")) {
                    tokens.emplace_back(TokenType::LEFT_SHIFT_EQUALS);
                    idx += 2;
                } else if (next_is("<")) {
                    tokens.emplace_back(TokenType::LEFT_SHIFT);
                    idx++;
                } else if (next_is("=")) {
                    tokens.emplace_back(TokenType::LESS_EQUALS);
                    idx++;
                } else {
                    tokens.emplace_back(TokenType::LESS);
                }
                break;
            case '>' :
                if (next_is(">=")) {
                    tokens.emplace_back(TokenType::RIGHT_SHIFT_EQUALS);
                    idx += 2;
                } else if (next_is(">")) {
                    tokens.emplace_back(TokenType::RIGHT_SHIFT);
                    idx++;
                } else if (next_is("=")) {
                    tokens.emplace_back(TokenType::GREATER_EQUALS);
                    idx++;
                } else {
                    tokens.emplace_back(TokenType::GREATER);
                }
                break;
            case '!' :
                if (next_is("=")) {
                    tokens.emplace_back(TokenType::NOT_EQUALS);
                    idx++;
                } else {
                    tokens.emplace_back(TokenType::NOT);
                }
                break;
            case '+' :
                if (next_is("+")) {
                    tokens.emplace_back(TokenType::INCREMENT);
                    idx++;
                } else if (next_is("=")) {
                    tokens.emplace_back(TokenType::PLUS_EQUALS);
                    idx++;
                } else {
                    tokens.emplace_back(TokenType::PLUS);
                }
                break;
            case '-' :
                if (next_is("-")) {
                    tokens.emplace_back(TokenType::DECREMENT);
                    idx++;
                } else if (next_is("=")) {
                    tokens.emplace_back(TokenType::MINUS_EQUALS);
                    idx++;
                } else {
                    tokens.emplace_back(TokenType::MINUS);
                }
                break;
            case '*' :
                if (next_is("=")) {
                    tokens.emplace_back(TokenType::STAR_EQUALS);
                    idx++;
                } else {
                    tokens.emplace_back(TokenType::STAR);
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
                    tokens.emplace_back(TokenType::SLASH_EQUALS);
                    idx++;
                } else {
                    tokens.emplace_back(TokenType::SLASH);
                }
                break;
            case '%' :
                if (next_is("=")) {
                    tokens.emplace_back(TokenType::MODULO_EQUALS);
                    idx++;
                } else {
                    tokens.emplace_back(TokenType::MODULO);
                }
                break;
            case '&' :
                if (next_is("&")) {
                    tokens.emplace_back(TokenType::LOGICAL_AND);
                    idx++;
                } else if (next_is("=")) {
                    tokens.emplace_back(TokenType::BITWISE_AND_EQUALS);
                    idx++;
                } else {
                    tokens.emplace_back(TokenType::BITWISE_AND);
                }
                break;
            case '|' :
                if (next_is("|")) {
                    tokens.emplace_back(TokenType::LOGICAL_OR);
                    idx++;
                } else if (next_is("=")) {
                    tokens.emplace_back(TokenType::BITWISE_OR_EQUALS);
                    idx++;
                } else {
                    tokens.emplace_back(TokenType::BITWISE_OR);
                }
                break;
            case '^' :
                if (next_is("=")) {
                    tokens.emplace_back(TokenType::BITWISE_XOR_EQUALS);
                    idx++;
                } else {
                    tokens.emplace_back(TokenType::BITWISE_XOR);
                }
                break;
            case '?' :
                tokens.emplace_back(TokenType::QUESTION);
                break;
            case '.' :
                tokens.emplace_back(TokenType::DOT);
                break;
            case '\"': {
                if (auto maybeToken = tokeniseString()) {
                    tokens.push_back(maybeToken.value());
                } else {
                    return std::nullopt;
                }
                break;
            }
            default:
                if (std::isdigit(c)) {
                    idx--;
                    Token token1 = tokeniseNumber();
                    if (next_is(".")) {
                        idx++;
                        Token token2 = tokeniseNumber();
                        float num = *token1.getIf<int>();
                        float decimal = *token2.getIf<int>();
                        assert(decimal >= 0);
                        if (decimal > 0) {
                            int digits = std::to_string(decimal).size();
                            decimal /= std::pow(10.0f, digits);
                        }
                        tokens.emplace_back(TokenType::FLOAT, num + decimal);
                    } else {
                        tokens.emplace_back(TokenType::INTEGER, *token1.getIf<int>());
                    }
                } else if (std::isalpha(c) || c == '_') {
                    idx--;
                    std::string identifier_like = extractIdentifierLike();
                    if (tokenMap.contains(identifier_like)) {
                        tokens.emplace_back(tokenMap.at(identifier_like));
                    } else {
                        tokens.emplace_back(TokenType::IDENTIFIER, identifier_like);
                    }
                } else {
                    return std::nullopt;
                }
        }
    }

    return tokens;
}


std::optional<Token> Tokeniser::tokeniseString() {
    std::string s;
    while (idx < str.length()) {
        char c = str[idx++];
        if (c == '\"') {
            return Token{TokenType::STRING_LITERAL, s};
        }
        s += c;
    }

    return std::nullopt;
}

Token Tokeniser::tokeniseNumber() {
    std::string s = next([](char c) {
            return !isdigit(c);
        });
    idx += s.length();
    return Token{TokenType::INTEGER, std::stoi(s)};
}

std::string Tokeniser::extractIdentifierLike() {
    std::string s = std::string{str[idx++]};
    const std::string rest_of_s = next([](char c){ return !(isalnum(c) || c == '_'); });
    idx += rest_of_s.length();
    s += rest_of_s;
    return s;
}

bool Tokeniser::next_is(const std::string &expected, const std::function<bool(char)> &until) const {
    const auto nextString = next(expected.length(), until);
    return nextString.size() == expected.length() && nextString == expected;
}

std::string Tokeniser::next(const std::function<bool(char)> &until) const {
    return next(str.length(), until);
}

std::string Tokeniser::next(std::size_t count, const std::function<bool(char)> &until) const {
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



