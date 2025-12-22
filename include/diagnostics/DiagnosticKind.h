//
// Created by Agamjeet Singh on 28/11/25.
//

#ifndef DIAGNOSTICKIND_H
#define DIAGNOSTICKIND_H

#include <magic_enum.hpp>
#include <optional>
#include <algorithm>
#include "../tokeniser/Token.h"
#include "../parser/Modifier.h"



enum class DiagnosticKind {
    UNTERMINATED_STRING_LITERAL,
    UNRECOGNISED_TOKEN,
    EXPECTED_DECLARATION,
    EXPECTED_CLASS_NAME,
    EXPECTED_SOMETHING,
    EXPECTED_IDENTIFIER,
    EXPECTED_SEMI_COLON,
    EXPECTED_LEFT_CURLY_BRACE,
    EXPECTED_RIGHT_CURLY_BRACE,
    EXPECTED_TYPEDEF,
    EXPECTED_GREATER,
    EXPECTED_RIGHT_PAREN,

    STATIC_NOT_VALID_MODIFIER_FOR_CLASSES,
    REPEATED_MODIFIER,
    ILLEGAL_MODIFIER_COMBINATION,
    MODIFIER_NOT_ALLOWED,
    TYPE_PARAMETERS_CANNOT_HAVE_GENERIC_ARGUMENTS
};

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

inline std::optional<std::string> notValidModifierDiagnosticToMsg(const DiagnosticKind kind) {
    std::string kindName = std::string{magic_enum::enum_name(kind)};
    
    // Look for pattern: <MODIFIER>_NOT_VALID_MODIFIER_FOR_<TARGET>
    const std::string pattern = "_NOT_VALID_MODIFIER_FOR_";
    auto patternPos = kindName.find(pattern);
    if (patternPos == std::string::npos) {
        return std::nullopt;
    }
    
    std::string modifierPart = kindName.substr(0, patternPos);
    std::string targetPart = kindName.substr(patternPos + pattern.length());
    
    auto modifier = magic_enum::enum_cast<Modifier>(modifierPart);
    if (!modifier.has_value()) {
        return std::nullopt;
    }
    
    // Convert target to lowercase
    std::string target = targetPart;
    std::ranges::transform(target, target.begin(), tolower);
    
    return "'" + toString(modifier.value()) + "' is not a valid modifier for " + target;
}

inline std::string toMsg(const DiagnosticKind kind, const std::string &str) {
    assert(kind == DiagnosticKind::TYPE_PARAMETERS_CANNOT_HAVE_GENERIC_ARGUMENTS);
    return "Type parameter " + str + " has generic arguments, which are not allowed";
}

inline std::string toMsg(const DiagnosticKind kind, Modifier modifier) {
    assert(kind == DiagnosticKind::REPEATED_MODIFIER || kind == DiagnosticKind::MODIFIER_NOT_ALLOWED);
    if (kind == DiagnosticKind::REPEATED_MODIFIER) return "Repeated modifier '" + toString(modifier) + "'";
    return "Modifier '" + toString(modifier) + "' not allowed here";
}

inline std::string toMsg(const DiagnosticKind kind, Modifier modifier1, Modifier modifier2) {
    assert(kind == DiagnosticKind::ILLEGAL_MODIFIER_COMBINATION);
    return "Illegal combination of modifiers '" + toString(modifier1) + "' and '" + toString(modifier2) + "'";
}

inline std::string toMsg(const DiagnosticKind kind) {
    switch (kind) {
        case DiagnosticKind::UNTERMINATED_STRING_LITERAL:
            return "Unterminated string literal";
        case DiagnosticKind::UNRECOGNISED_TOKEN:
            return "Unrecognised token";
        case DiagnosticKind::EXPECTED_DECLARATION:
            return "Expected declaration";
        case DiagnosticKind::EXPECTED_CLASS_NAME:
            return "Expected class name";
        case DiagnosticKind::EXPECTED_IDENTIFIER:
            return "Expected identifier";
        default:
            if (auto msg = expectedDiagnostictoMsg(kind)) {
                return msg.value();
            }
            
            if (auto msg = notValidModifierDiagnosticToMsg(kind)) {
                return msg.value();
            }

            throw std::runtime_error("Kind cannot be converted to msg");
    }
}

#endif //DIAGNOSTICKIND_H
