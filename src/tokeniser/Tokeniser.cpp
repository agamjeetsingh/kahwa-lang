//
// Created by Agamjeet Singh on 22/11/25.
//

#include "../../include/tokeniser/Tokeniser.h"

#include "../../include/parser/exceptions/lexical/UnrecognisedTokenError.h"
#include "../../include/parser/exceptions/lexical/UnterminatedStringLiteralError.h"

std::optional<std::vector<Token> > Tokeniser::tokenise() {
    while (idx < str.length()) {
        std::size_t curr_idx = idx;
        char c = str[idx++];
        if (delimiters.contains(c)) continue;

        switch (c) {
            case ':' :
                tokens.emplace_back(TokenType::COLON, curr_idx);
                break;
            case ';' :
                tokens.emplace_back(TokenType::SEMI_COLON, curr_idx);
                break;
            case ',' :
                tokens.emplace_back(TokenType::COMMA, curr_idx);
                break;
            case '{' :
                tokens.emplace_back(TokenType::LEFT_CURLY_BRACE, curr_idx);
                break;
            case '}' :
                tokens.emplace_back(TokenType::RIGHT_CURLY_BRACE, curr_idx);
                break;
            case '(' :
                tokens.emplace_back(TokenType::LEFT_PAREN, curr_idx);
                break;
            case ')' :
                tokens.emplace_back(TokenType::RIGHT_PAREN, curr_idx);
                break;
            case '[' :
                tokens.emplace_back(TokenType::LEFT_BRACKET, curr_idx);
                break;
            case ']' :
                tokens.emplace_back(TokenType::RIGHT_BRACKET, curr_idx);
                break;
            case '=' :
                if (next_is("=")) {
                    tokens.emplace_back(TokenType::DOUBLE_EQUALS, curr_idx, 2);
                    idx++;
                } else {
                    tokens.emplace_back(TokenType::EQUALS, curr_idx);
                }
                break;
            case '<' :
                if (next_is("<=")) {
                    tokens.emplace_back(TokenType::LEFT_SHIFT_EQUALS, curr_idx, 3);
                    idx += 2;
                } else if (next_is("<")) {
                    tokens.emplace_back(TokenType::LEFT_SHIFT, curr_idx, 2);
                    idx++;
                } else if (next_is("=")) {
                    tokens.emplace_back(TokenType::LESS_EQUALS, curr_idx, 2);
                    idx++;
                } else {
                    tokens.emplace_back(TokenType::LESS, curr_idx);
                }
                break;
            case '>' :
                if (next_is(">=")) {
                    tokens.emplace_back(TokenType::RIGHT_SHIFT_EQUALS, curr_idx, 3);
                    idx += 2;
                } else if (next_is(">")) {
                    tokens.emplace_back(TokenType::RIGHT_SHIFT, curr_idx, 2);
                    idx++;
                } else if (next_is("=")) {
                    tokens.emplace_back(TokenType::GREATER_EQUALS, curr_idx, 2);
                    idx++;
                } else {
                    tokens.emplace_back(TokenType::GREATER, curr_idx);
                }
                break;
            case '!' :
                if (next_is("=")) {
                    tokens.emplace_back(TokenType::NOT_EQUALS, curr_idx, 2);
                    idx++;
                } else {
                    tokens.emplace_back(TokenType::NOT, curr_idx);
                }
                break;
            case '+' :
                if (next_is("+")) {
                    tokens.emplace_back(TokenType::INCREMENT, curr_idx, 2);
                    idx++;
                } else if (next_is("=")) {
                    tokens.emplace_back(TokenType::PLUS_EQUALS, curr_idx, 2);
                    idx++;
                } else {
                    tokens.emplace_back(TokenType::PLUS, curr_idx, 1);
                }
                break;
            case '-' :
                if (next_is("-")) {
                    tokens.emplace_back(TokenType::DECREMENT, curr_idx, 2);
                    idx++;
                } else if (next_is("=")) {
                    tokens.emplace_back(TokenType::MINUS_EQUALS, curr_idx, 2);
                    idx++;
                } else {
                    tokens.emplace_back(TokenType::MINUS, curr_idx);
                }
                break;
            case '*' :
                if (next_is("=")) {
                    tokens.emplace_back(TokenType::STAR_EQUALS, curr_idx, 2);
                    idx++;
                } else {
                    tokens.emplace_back(TokenType::STAR, curr_idx);
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
                    tokens.emplace_back(TokenType::SLASH_EQUALS, curr_idx, 2);
                    idx++;
                } else {
                    tokens.emplace_back(TokenType::SLASH, curr_idx);
                }
                break;
            case '%' :
                if (next_is("=")) {
                    tokens.emplace_back(TokenType::MODULO_EQUALS, curr_idx, 2);
                    idx++;
                } else {
                    tokens.emplace_back(TokenType::MODULO, curr_idx);
                }
                break;
            case '&' :
                if (next_is("&")) {
                    tokens.emplace_back(TokenType::LOGICAL_AND, curr_idx, 2);
                    idx++;
                } else if (next_is("=")) {
                    tokens.emplace_back(TokenType::BITWISE_AND_EQUALS, curr_idx, 2);
                    idx++;
                } else {
                    tokens.emplace_back(TokenType::BITWISE_AND, curr_idx);
                }
                break;
            case '|' :
                if (next_is("|")) {
                    tokens.emplace_back(TokenType::LOGICAL_OR, curr_idx, 2);
                    idx++;
                } else if (next_is("=")) {
                    tokens.emplace_back(TokenType::BITWISE_OR_EQUALS, curr_idx, 2);
                    idx++;
                } else {
                    tokens.emplace_back(TokenType::BITWISE_OR, curr_idx);
                }
                break;
            case '^' :
                if (next_is("=")) {
                    tokens.emplace_back(TokenType::BITWISE_XOR_EQUALS, curr_idx, 2);
                    idx++;
                } else {
                    tokens.emplace_back(TokenType::BITWISE_XOR, curr_idx);
                }
                break;
            case '?' :
                tokens.emplace_back(TokenType::QUESTION, curr_idx);
                break;
            case '.' :
                tokens.emplace_back(TokenType::DOT, curr_idx);
                break;
            case '\"': {
                if (auto maybeToken = tokeniseString(curr_idx)) {
                    tokens.push_back(maybeToken.value());
                } else {
                    throw UnterminatedStringLiteralError();
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
                        tokens.emplace_back(TokenType::FLOAT, num + decimal, curr_idx, std::to_string(num + decimal).length());
                    } else {
                        int num = *token1.getIf<int>();
                        tokens.emplace_back(TokenType::INTEGER, num, curr_idx, std::to_string(num).length());
                    }
                } else if (std::isalpha(c) || c == '_') {
                    idx--;
                    std::string identifier_like = extractIdentifierLike();
                    if (tokenMap.contains(identifier_like)) {
                        tokens.emplace_back(tokenMap.at(identifier_like), curr_idx, identifier_like.length());
                    } else {
                        tokens.emplace_back(TokenType::IDENTIFIER, identifier_like, curr_idx, identifier_like.length());
                    }
                } else {
                    throw UnrecognisedTokenError();
                }
        }
    }

    return tokens;
}


std::optional<Token> Tokeniser::tokeniseString(std::size_t curr_idx) {
    std::string s;
    while (idx < str.length()) {
        char c = str[idx++];
        if (c == '\"') {
            return Token{TokenType::STRING_LITERAL, s, curr_idx, s.length() + 2};
        }
        s += c;
    }

    return std::nullopt;
}

Token Tokeniser::tokeniseNumber(std::size_t curr_idx) {
    std::string s = next([](char c) {
            return !isdigit(c);
        });
    idx += s.length();
    return Token{TokenType::INTEGER, std::stoi(s), curr_idx, s.length()};
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



