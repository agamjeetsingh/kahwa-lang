//
// Created by Agamjeet Singh on 30/12/25.
//

#ifndef LSPTOKENMODIFIER_H
#define LSPTOKENMODIFIER_H
#include <string>
#include <magic_enum.hpp>

enum class LSPTokenModifier {
    DECLARATION,
    STATIC,
    ABSTRACT,
};

inline std::string toString(LSPTokenModifier tokenModifier) {
    switch (tokenModifier) {
        case LSPTokenModifier::DECLARATION:
            return "declaration";
        case LSPTokenModifier::STATIC:
            return "static";
        case LSPTokenModifier::ABSTRACT:
            return "abstract";
    }
}

inline std::vector<std::string> getSupportedTokenModifiers() {
    std::vector<std::string> res;
    res.reserve(magic_enum::enum_values<LSPTokenModifier>().size());

    std::ranges::transform(magic_enum::enum_values<LSPTokenModifier>(), std::back_inserter(res), [](LSPTokenModifier modifier) { return toString(modifier); });

    return res;
}


#endif //LSPTOKENMODIFIER_H
