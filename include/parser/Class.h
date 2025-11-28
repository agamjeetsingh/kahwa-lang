//
// Created by Agamjeet Singh on 23/11/25.
//

#ifndef CLASS_H
#define CLASS_H
#include <optional>
#include <utility>

#include "ExtensibilityModifier.h"
#include "VisibilityModifier.h"
#include "../diagnostics/semantic/ModifierError.h"

class Class {
public:
    explicit Class(std::optional<std::shared_ptr<Class>> parentClass = std::nullopt) : parentClass(std::move(parentClass)) {}

    void addVisibility(VisibilityModifier new_modifier) {
        if (visibility_modifier) {
            if (visibility_modifier.value() == new_modifier) {
                throw RepeatedModifierError(toString(new_modifier));
            }

            throw IllegalModifierCombinationError(toString(visibility_modifier.value()), toString(new_modifier));
        }
        visibility_modifier = new_modifier;
    }

    void setOpen() {
        if (extensibility_modifier) {
            if (extensibility_modifier.value() == ExtensibilityModifier::FINAL) {
                throw IllegalModifierCombinationError(toString(extensibility_modifier.value()), toString(ExtensibilityModifier::OPEN));
            }
            if (!isAbstract) {
                throw RepeatedModifierError(toString(ExtensibilityModifier::OPEN));
            }
            return;
        }
        extensibility_modifier = ExtensibilityModifier::OPEN;
    }

    void setAbstract() {
        if (isAbstract) throw RepeatedModifierError("abstract");
        isAbstract = true;
        extensibility_modifier = ExtensibilityModifier::OPEN;
    }

    void setFinal() {
        if (isAbstract) {
            throw IllegalModifierCombinationError("abstract", toString(ExtensibilityModifier::FINAL));
        }
        if (extensibility_modifier) {
            if (extensibility_modifier.value() == ExtensibilityModifier::OPEN) {
                throw IllegalModifierCombinationError(toString(extensibility_modifier.value()), toString(ExtensibilityModifier::FINAL));
            }

            throw RepeatedModifierError(toString(ExtensibilityModifier::FINAL));
        }
        extensibility_modifier = ExtensibilityModifier::FINAL;
    }

private:
    bool isAbstract = false;

    [[nodiscard]] bool isTopLevel() const {
        return !parentClass.has_value();
    }
    [[nodiscard]] VisibilityModifier getVisibility() const {
        return visibility_modifier.has_value() ? visibility_modifier.value() : DEFAULT_VISIBILITY;
    }
    [[nodiscard]] bool isExtensible() const {
        return isOpen(extensibility_modifier.has_value() ? extensibility_modifier.value() : DEFAULT_EXTENSIBILITY);
    }

    std::optional<std::shared_ptr<Class>> parentClass;
    std::optional<VisibilityModifier> visibility_modifier;
    std::optional<ExtensibilityModifier> extensibility_modifier;

    static constexpr auto DEFAULT_VISIBILITY = VisibilityModifier::PUBLIC;
    static constexpr auto DEFAULT_EXTENSIBILITY = ExtensibilityModifier::FINAL;
};



#endif //CLASS_H
