//
// Created by Agamjeet Singh on 30/12/25.
//

#ifndef LSPTOKENTYPE_H
#define LSPTOKENTYPE_H
#include <vector>
#include <magic_enum.hpp>

enum class LSPTokenType {
    TYPE,
    CLASS,
    INTERFACE,
    TYPE_PARAMETER,
    PARAMETER,
    VARIABLE,
    PROPERTY,
    FUNCTION,
    METHOD,
    KEYWORD,
    MODIFIER,
    COMMENT,
    STRING,
    NUMBER,
    OPERATOR
};

inline std::string toString(LSPTokenType tokenType) {
    switch (tokenType) {
        case LSPTokenType::TYPE:
            return "type";
        case LSPTokenType::CLASS:
            return "class";
        case LSPTokenType::INTERFACE:
            return "interface";
        case LSPTokenType::TYPE_PARAMETER:
            return "typeParameter";
        case LSPTokenType::PARAMETER:
            return "parameter";
        case LSPTokenType::VARIABLE:
            return "variable";
        case LSPTokenType::PROPERTY:
            return "property";
        case LSPTokenType::FUNCTION:
            return "function";
        case LSPTokenType::METHOD:
            return "method";
        case LSPTokenType::KEYWORD:
            return "keyword";
        case LSPTokenType::MODIFIER:
            return "modifier";
        case LSPTokenType::COMMENT:
            return "comment";
        case LSPTokenType::STRING:
            return "string";
        case LSPTokenType::NUMBER:
            return "number";
        case LSPTokenType::OPERATOR:
            return "operator";
    }
}

inline std::vector<std::string> getSupportedTokenTypes() {
    std::vector<std::string> res;
    res.reserve(magic_enum::enum_values<LSPTokenType>().size());

    std::ranges::transform(magic_enum::enum_values<LSPTokenType>(), std::back_inserter(res), [](LSPTokenType type) { return toString(type); });

    return res;
}

#endif //LSPTOKENTYPE_H
