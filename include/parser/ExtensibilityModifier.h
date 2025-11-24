//
// Created by Agamjeet Singh on 23/11/25.
//

#ifndef EXTENSIBILITYMODIFIER_H
#define EXTENSIBILITYMODIFIER_H
#include <string>


enum class ExtensibilityModifier {
    OPEN,
    FINAL
};

inline bool isOpen(ExtensibilityModifier modifier) {
    switch (modifier) {
        case ExtensibilityModifier::OPEN:
            return true;
        case ExtensibilityModifier::FINAL:
            return false;
        default:
            throw std::runtime_error("Unknown ExtensibilityModifier");
    }
}

inline std::string toString(const ExtensibilityModifier modifier) {
    switch (modifier) {
        case ExtensibilityModifier::OPEN:
            return "open";
        case ExtensibilityModifier::FINAL:
            return "final";
        default:
            throw std::runtime_error("Unknown ExtensibilityModifier");
    }
}

#endif //EXTENSIBILITYMODIFIER_H
