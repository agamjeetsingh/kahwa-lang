//
// Created by Agamjeet Singh on 19/12/25.
//

#ifndef SEMANTICANALYSER_H
#define SEMANTICANALYSER_H
#include <ranges>

#include "../parser/ClassDecl.h"
#include "../diagnostics/DiagnosticEngine.h"
#include "ClassSymbol.h"


class DiagnosticEngine;

class SemanticAnalyser {
public:
    explicit SemanticAnalyser(Arena& astArena, DiagnosticEngine& diagnosticEngine):
    astArena(astArena),
    diagnosticEngine(diagnosticEngine) {
        SymbolBuilder::setArena(&astArena);
    }



    ClassSymbol* analyseClass(const ClassDecl* classDecl, bool topLevel = true) {
        ClassSymbolBuilder builder{classDecl->name};

        // Adding all methods
        std::vector<MethodSymbol*> methods{classDecl->methods.size()};
        std::ranges::transform(classDecl->methods, methods.begin(), [this](const MethodDecl* methodDecl) {
            return analyseMethod(methodDecl);
        });
        builder.with(methods);

        // Adding all nested classes (recursively)
        std::vector<ClassSymbol*> nestedClasses{classDecl->nestedClasses.size()};
        std::ranges::transform(classDecl->nestedClasses, nestedClasses.begin(), [this](const ClassDecl* nestedClass) {
            return analyseClass(nestedClass, false);
        });
        builder.withNestedClasses(nestedClasses);

        // Setting effective visibility
        auto visibilityModifiers = classDecl->modifiers | std::ranges::views::filter([](const ModifierNode& modifier) {
            return isVisibilityModifier(modifier.modifier);
        });
        builder.setVisibility(resolveVisibility(std::vector(visibilityModifiers.begin(), visibilityModifiers.end()), topLevel));

        // Static keyword isn't allowed. Emitting diagnostics for each occurence.
        for (auto staticModifier : classDecl->modifiers |
            std::ranges::views::filter([](const ModifierNode& modifier) {
                return modifier.modifier == Modifier::STATIC;
            })) {
            diagnosticEngine.reportProblem(
                DiagnosticSeverity::ERROR,
                DiagnosticKind::MODIFIER_NOT_ALLOWED,
                staticModifier.sourceRange,
                toMsg(DiagnosticKind::MODIFIER_NOT_ALLOWED, staticModifier.modifier));
        }

        // Setting effective modality
        auto modalityModifiers = classDecl->modifiers | std::ranges::views::filter([](const ModifierNode& modifier) {
            return isModalityModifier(modifier.modifier);
        });
        builder.setModality(resolveModality(std::vector(modalityModifiers.begin(), modalityModifiers.end())));

        return builder.build();
    }


    MethodSymbol* analyseMethod(const MethodDecl* methodDecl) {
        return nullptr; // TODO
    }

private:
    Arena& astArena;
    DiagnosticEngine& diagnosticEngine;

    [[nodiscard]] Modifier resolveModality(const std::vector<ModifierNode> &modalityModifiers) const {
        Modifier effectiveModality = Modifier::FINAL;
        if (!modalityModifiers.empty()) {
            if (std::ranges::any_of(modalityModifiers, [](const ModifierNode& modifierNode) {
               return modifierNode.modifier == Modifier::ABSTRACT;
            })) {
                effectiveModality = Modifier::ABSTRACT;
            } else if (std::ranges::any_of(modalityModifiers, [](const ModifierNode& modifierNode) {
                return modifierNode.modifier == Modifier::OPEN;
             })) {
               effectiveModality = Modifier::OPEN;
           }
        }

        bool finalFound = false;
        bool openFound = false;
        bool abstractFound = false;

        for (const auto modifierNode: modalityModifiers) {
            switch (modifierNode.modifier) {
                case Modifier::FINAL:
                    if (abstractFound || openFound) {
                        diagnosticEngine.reportProblem(
                            DiagnosticSeverity::ERROR,
                            DiagnosticKind::ILLEGAL_MODIFIER_COMBINATION,
                            modifierNode.sourceRange,
                            toMsg(DiagnosticKind::ILLEGAL_MODIFIER_COMBINATION, abstractFound ? Modifier::ABSTRACT : Modifier::OPEN, Modifier::FINAL));
                    } else if (finalFound) {
                        diagnosticEngine.reportProblem(
                            DiagnosticSeverity::ERROR,
                            DiagnosticKind::REPEATED_MODIFIER,
                            modifierNode.sourceRange,
                            toMsg(DiagnosticKind::REPEATED_MODIFIER, Modifier::FINAL));
                    }
                    finalFound = true;
                    break;

                case Modifier::ABSTRACT:
                    if (finalFound) {
                        diagnosticEngine.reportProblem(
                            DiagnosticSeverity::ERROR,
                            DiagnosticKind::ILLEGAL_MODIFIER_COMBINATION,
                            modifierNode.sourceRange,
                            toMsg(DiagnosticKind::ILLEGAL_MODIFIER_COMBINATION, Modifier::FINAL, Modifier::ABSTRACT));
                    } else if (abstractFound) {
                        diagnosticEngine.reportProblem(
                            DiagnosticSeverity::ERROR,
                            DiagnosticKind::REPEATED_MODIFIER,
                            modifierNode.sourceRange,
                            toMsg(DiagnosticKind::REPEATED_MODIFIER, Modifier::ABSTRACT));
                    }
                    abstractFound = true;
                    break;

                case Modifier::OPEN:
                    if (finalFound) {
                        diagnosticEngine.reportProblem(
                            DiagnosticSeverity::ERROR,
                            DiagnosticKind::ILLEGAL_MODIFIER_COMBINATION,
                            modifierNode.sourceRange,
                            toMsg(DiagnosticKind::ILLEGAL_MODIFIER_COMBINATION, Modifier::FINAL, Modifier::OPEN));
                    } else if (openFound) {
                        diagnosticEngine.reportProblem(
                            DiagnosticSeverity::ERROR,
                            DiagnosticKind::REPEATED_MODIFIER,
                            modifierNode.sourceRange,
                            toMsg(DiagnosticKind::REPEATED_MODIFIER, Modifier::OPEN));
                    }
                    openFound = true;
                    break;
                default: throw std::runtime_error("resolveModality() received a non-visibility modifier: " + toString(modifierNode.modifier));;
            }
        }

        return effectiveModality;
    }

    [[nodiscard]] Modifier resolveVisibility(const std::vector<ModifierNode> &visibilityModifiers, bool topLevel) const {
        Modifier res;

        if (visibilityModifiers.empty()) {
            res = topLevel ? Modifier::PUBLIC : Modifier::PRIVATE;
        } else if (visibilityModifiers.size() == 1) {
            res = topLevel && visibilityModifiers[0].modifier == Modifier::PROTECTED ? Modifier::PUBLIC : visibilityModifiers[0].modifier;
        } else {
            res = visibilityModifiers[0].modifier;
        }

        bool publicFound = false;
        bool protectedFound = false;
        bool privateFound = false;

        for (const auto modifierNode: visibilityModifiers) {
            switch (modifierNode.modifier) {
                case Modifier::PUBLIC:
                    if ((!topLevel && protectedFound) || privateFound) {
                        diagnosticEngine.reportProblem(
                            DiagnosticSeverity::ERROR,
                            DiagnosticKind::ILLEGAL_MODIFIER_COMBINATION,
                            modifierNode.sourceRange,
                            toMsg(DiagnosticKind::ILLEGAL_MODIFIER_COMBINATION, protectedFound ? Modifier::PROTECTED : Modifier::PRIVATE, Modifier::PUBLIC));
                    } else if (publicFound) {
                        diagnosticEngine.reportProblem(
                            DiagnosticSeverity::ERROR,
                            DiagnosticKind::REPEATED_MODIFIER,
                            modifierNode.sourceRange,
                            toMsg(DiagnosticKind::REPEATED_MODIFIER, Modifier::PUBLIC));
                    }
                    publicFound = true;
                    break;

                case Modifier::PRIVATE:
                    if ((!topLevel && protectedFound) || publicFound) {
                        diagnosticEngine.reportProblem(
                            DiagnosticSeverity::ERROR,
                            DiagnosticKind::ILLEGAL_MODIFIER_COMBINATION,
                            modifierNode.sourceRange,
                            toMsg(DiagnosticKind::ILLEGAL_MODIFIER_COMBINATION, protectedFound ? Modifier::PROTECTED : Modifier::PUBLIC, Modifier::PRIVATE));
                    } else if (privateFound) {
                        diagnosticEngine.reportProblem(
                            DiagnosticSeverity::ERROR,
                            DiagnosticKind::REPEATED_MODIFIER,
                            modifierNode.sourceRange,
                            toMsg(DiagnosticKind::REPEATED_MODIFIER, Modifier::PRIVATE));
                    }
                    privateFound = true;
                    break;

                case Modifier::PROTECTED:
                    if (topLevel) {
                        diagnosticEngine.reportProblem(
                            DiagnosticSeverity::ERROR,
                            DiagnosticKind::MODIFIER_NOT_ALLOWED,
                            modifierNode.sourceRange,
                            toMsg(DiagnosticKind::MODIFIER_NOT_ALLOWED, Modifier::PROTECTED));
                    } else if (publicFound || privateFound) {
                        diagnosticEngine.reportProblem(
                            DiagnosticSeverity::ERROR,
                            DiagnosticKind::ILLEGAL_MODIFIER_COMBINATION,
                            modifierNode.sourceRange,
                            toMsg(DiagnosticKind::ILLEGAL_MODIFIER_COMBINATION, publicFound ? Modifier::PUBLIC : Modifier::PRIVATE, Modifier::PROTECTED));
                    } else if (protectedFound) {
                        diagnosticEngine.reportProblem(
                            DiagnosticSeverity::ERROR,
                            DiagnosticKind::REPEATED_MODIFIER,
                            modifierNode.sourceRange,
                            toMsg(DiagnosticKind::REPEATED_MODIFIER, Modifier::PROTECTED));
                    }
                    protectedFound = true;
                    break;

                default: throw std::runtime_error("resolveVisibility() received a non-visibility modifier: " + toString(modifierNode.modifier));
            }
        }

        return res;
    }
};



#endif //SEMANTICANALYSER_H
