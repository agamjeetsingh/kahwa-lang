//
// Created by Agamjeet Singh on 28/11/25.
//

#ifndef DIAGNOSTICKIND_H
#define DIAGNOSTICKIND_H



enum class DiagnosticKind {
    UNTERMINATED_STRING_LITERAL,
    UNRECOGNISED_TOKEN,
};

inline std::string toMsg(DiagnosticKind kind) {
    switch (kind) {
        case DiagnosticKind::UNTERMINATED_STRING_LITERAL:
            return "Unterminated string literal.";
        case DiagnosticKind::UNRECOGNISED_TOKEN:
            return "Unrecognised token.";
        default:
            throw std::runtime_error("Kind cannot be converted to msg.");
    }
}

#endif //DIAGNOSTICKIND_H
