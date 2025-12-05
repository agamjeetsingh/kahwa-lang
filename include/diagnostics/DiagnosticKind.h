//
// Created by Agamjeet Singh on 28/11/25.
//

#ifndef DIAGNOSTICKIND_H
#define DIAGNOSTICKIND_H

#include <magic_enum.hpp>
#include <optional>
#include "../tokeniser/Token.h"



enum class DiagnosticKind {
    UNTERMINATED_STRING_LITERAL,
    UNRECOGNISED_TOKEN,
    EXPECTED_DECLARATION,
    EXPECTED_CLASS_NAME,
    EXPECTED_SOMETHING,
    EXPECTED_IDENTIFIER,
    EXPECTED_SEMI_COLON,
    EXPECTED_LEFT_CURLY_BRACE,
    EXPECTED_RIGHT_CURLY_BRACE
};

inline std::string toMsg(const DiagnosticKind kind, const std::string& aux) {
    switch (kind) {
        case DiagnosticKind::EXPECTED_SOMETHING:
            return "Expected '" + aux + "'";
        default:
            throw std::runtime_error("Kind cannot be converted to msg with aux.");
    }
}

inline DiagnosticKind expectedTokenTypeToDiagnosticKind(const TokenType tokenType) {
    const auto typeName = std::string{magic_enum::enum_name(tokenType)};

    // TODO - Ensure each token type has one, or throw error on failure
    return magic_enum::enum_cast<DiagnosticKind>("EXPECTED_" + typeName).value();
}

inline std::optional<TokenType> expectedDiagnosticToTokenType(const DiagnosticKind kind) {
    std::string kindName = std::string{magic_enum::enum_name(kind)};

    if (!kindName.starts_with("EXPECTED_")) {
        return std::nullopt;
    }

    std::string tokenPart = kindName.substr(9);

    return magic_enum::enum_cast<TokenType>(tokenPart);
}

inline std::optional<std::string> expectedDiagnostictoMsg(const DiagnosticKind kind) {
    std::string kindName = std::string{magic_enum::enum_name(kind)};
    
    if (!kindName.starts_with("EXPECTED_")) {
        return std::nullopt;
    }
    
    std::string tokenPart = kindName.substr(9);
    
    auto tokenType = magic_enum::enum_cast<TokenType>(tokenPart);
    if (!tokenType.has_value()) {
        return std::nullopt;
    }
    
    std::string tokenStr = tokenTypeToString(tokenType.value());
    
    if (tokenType.value() == TokenType::IDENTIFIER) {
        return "Expected identifier";
    }
    
    if (tokenStr.size() == 1 || (tokenStr.size() > 1 && !std::isalpha(tokenStr[0]))) {
        return "Expected '" + tokenStr + "'";
    }
    
    return "Expected " + tokenStr;
}

inline std::string toMsg(const DiagnosticKind kind) {
    switch (kind) {
        case DiagnosticKind::UNTERMINATED_STRING_LITERAL:
            return "Unterminated string literal.";
        case DiagnosticKind::UNRECOGNISED_TOKEN:
            return "Unrecognised token.";
        case DiagnosticKind::EXPECTED_DECLARATION:
            return "Expected a declaration.";
        case DiagnosticKind::EXPECTED_CLASS_NAME:
            return "Expected a class name.";
        case DiagnosticKind::EXPECTED_IDENTIFIER:
            return "Expected an identifier.";
        default:
            if (auto msg = expectedDiagnostictoMsg(kind)) {
                return msg.value();
            }

            throw std::runtime_error("Kind cannot be converted to msg.");
    }
}

#endif //DIAGNOSTICKIND_H
