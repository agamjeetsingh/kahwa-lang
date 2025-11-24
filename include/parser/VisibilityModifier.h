//
// Created by Agamjeet Singh on 23/11/25.
//

#ifndef VISIBILITYMODIFIER_H
#define VISIBILITYMODIFIER_H
#include <string>

enum class VisibilityModifier {
    PUBLIC,
    PRIVATE,
    PROTECTED
};

inline std::string toString(const VisibilityModifier modifier) {
    switch (modifier) {
        case VisibilityModifier::PUBLIC:
            return "public";
        case VisibilityModifier::PRIVATE:
            return "private";
        case VisibilityModifier::PROTECTED:
            return "protected";
        default:
            throw std::runtime_error("Unknown VisibilityModifier");
    }
}

#endif //VISIBILITYMODIFIER_H
