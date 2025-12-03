//
// Created by Agamjeet Singh on 28/11/25.
//

#ifndef DIAGNOSTICKIND_H
#define DIAGNOSTICKIND_H



enum class DiagnosticKind {
    UNTERMINATED_STRING_LITERAL,
    UNRECOGNISED_TOKEN,
    EXPECTED_DECLARATION,
    EXPECTED_CLASS_NAME,
    EXPECTED_SOMETHING
};

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
        default:
            throw std::runtime_error("Kind cannot be converted to msg.");
    }
}

inline std::string toMsg(const DiagnosticKind kind, const std::string& aux) {
    switch (kind) {
        case DiagnosticKind::EXPECTED_SOMETHING:
            return "Expected '" + aux + "'";
        default:
            throw std::runtime_error("Kind cannot be converted to msg with aux.");
    }
}

#endif //DIAGNOSTICKIND_H
