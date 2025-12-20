//
// Created by Agamjeet Singh on 19/12/25.
//

#ifndef SEMANTICANALYSER_H
#define SEMANTICANALYSER_H
#include <ranges>
#include <__ranges/views.h>

#include "ClassDecl.h"
#include "../diagnostics/DiagnosticEngine.h"
#include "../symbols/ClassSymbol.h"


class DiagnosticEngine;

class SemanticAnalyser {
public:
    explicit SemanticAnalyser(Arena& astArena, DiagnosticEngine& diagnosticEngine):
    astArena(astArena),
    diagnosticEngine(diagnosticEngine) {
        SymbolBuilder::setArena(&astArena);
    }

    ClassSymbol* analyseClass(const ClassDecl* classDecl) {
        ClassSymbolBuilder builder{classDecl->name};

        std::vector<MethodSymbol*> methods{classDecl->methods.size()};
        std::ranges::transform(classDecl->methods, methods.begin(), [this](const MethodDecl* methodDecl) {
            return analyseMethod(methodDecl);
        });
        builder.with(methods);

        std::vector<ClassSymbol*> nestedClasses{classDecl->nestedClasses.size()};
        std::ranges::transform(classDecl->nestedClasses, nestedClasses.begin(), [this](const ClassDecl* nestedClass) {
            return analyseClass(nestedClass);
        });
        builder.withNestedClasses(nestedClasses);

        auto filtered = classDecl->modifiers | std::ranges::views::filter([](const Modifier& modifier) {
        return std::unordered_set{Modifier::PRIVATE, Modifier::PROTECTED, Modifier::PUBLIC}.contains(modifier);
        });
        builder.setVisibility(resolveVisibility(std::vector(filtered.begin(), filtered.end()), ))

        // Analyse modifiers
        std::unordered_map<Modifier, std::vector<int>> modifierMap;
        for (int i = 0; i < classDecl->modifiers.size(); i++) {
            modifierMap[classDecl->modifiers[i]].push_back(i);
        }

        if (modifierMap.contains(Modifier::STATIC)) {
            for (const int idx: modifierMap[Modifier::STATIC]) {
                diagnosticEngine.reportProblem(
                    DiagnosticSeverity::ERROR,
                    DiagnosticKind::MODIFIER_NOT_ALLOWED,
                    classDecl->modifierSourceRanges[idx],
                    toMsg(DiagnosticKind::MODIFIER_NOT_ALLOWED, Modifier::STATIC));
            }
        }

        return builder.build();
    }


    MethodSymbol* analyseMethod(const MethodDecl* methodDecl) {
        return nullptr; // TODO
    }

private:
    Arena& astArena;
    DiagnosticEngine& diagnosticEngine;

    Modifier resolveVisibility(const std::vector<Modifier> &visibilityModifiers, const std::vector<SourceRange> &sourceRanges, bool topLevel) const {
        const Modifier res = visibilityModifiers.size() == 1 ?
        (topLevel && visibilityModifiers[0] == Modifier::PROTECTED ? Modifier::PUBLIC : visibilityModifiers[0])
        : (topLevel ? Modifier::PUBLIC : Modifier::PRIVATE);

        bool publicFound = false;
        bool protectedFound = false;
        bool privateFound = false;

        for (int i = 0; i < visibilityModifiers.size(); i++) {
            switch (const auto modifier = visibilityModifiers[i]) {
                case Modifier::PUBLIC:
                    if ((!topLevel && protectedFound) || privateFound) {
                        diagnosticEngine.reportProblem(
                            DiagnosticSeverity::ERROR,
                            DiagnosticKind::ILLEGAL_MODIFIER_COMBINATION,
                            sourceRanges[i],
                            toMsg(DiagnosticKind::ILLEGAL_MODIFIER_COMBINATION, protectedFound ? Modifier::PROTECTED : Modifier::PRIVATE));
                    } else if (publicFound) {
                        diagnosticEngine.reportProblem(
                            DiagnosticSeverity::ERROR,
                            DiagnosticKind::REPEATED_MODIFIER,
                            sourceRanges[i],
                            toMsg(DiagnosticKind::REPEATED_MODIFIER, Modifier::PUBLIC));
                    } else {
                        publicFound = true;
                    }

                    break;

                case Modifier::PRIVATE:
                    if ((!topLevel && protectedFound) || publicFound) {
                        diagnosticEngine.reportProblem(
                            DiagnosticSeverity::ERROR,
                            DiagnosticKind::ILLEGAL_MODIFIER_COMBINATION,
                            sourceRanges[i],
                            toMsg(DiagnosticKind::ILLEGAL_MODIFIER_COMBINATION, protectedFound ? Modifier::PROTECTED : Modifier::PUBLIC));
                    } else if (privateFound) {
                        diagnosticEngine.reportProblem(
                            DiagnosticSeverity::ERROR,
                            DiagnosticKind::REPEATED_MODIFIER,
                            sourceRanges[i],
                            toMsg(DiagnosticKind::REPEATED_MODIFIER, Modifier::PRIVATE));
                    } else {
                        privateFound = true;
                    }
                    break;

                case Modifier::PROTECTED:
                    if (topLevel) {
                        diagnosticEngine.reportProblem(
                            DiagnosticSeverity::ERROR,
                            DiagnosticKind::MODIFIER_NOT_ALLOWED,
                            sourceRanges[i],
                            toMsg(DiagnosticKind::MODIFIER_NOT_ALLOWED, Modifier::PROTECTED));
                    } else if (publicFound || privateFound) {
                        diagnosticEngine.reportProblem(
                            DiagnosticSeverity::ERROR,
                            DiagnosticKind::ILLEGAL_MODIFIER_COMBINATION,
                            sourceRanges[i],
                            toMsg(DiagnosticKind::ILLEGAL_MODIFIER_COMBINATION, protectedFound ? Modifier::PROTECTED : Modifier::PRIVATE));
                    } else if (protectedFound) {
                        diagnosticEngine.reportProblem(
                            DiagnosticSeverity::ERROR,
                            DiagnosticKind::REPEATED_MODIFIER,
                            sourceRanges[i],
                            toMsg(DiagnosticKind::REPEATED_MODIFIER, Modifier::PROTECTED));
                    } else {
                        protectedFound = true;
                    }
                    break;

                default: throw std::runtime_error("resolveVisibility() received a non-visibility modifier: " + toString(modifier));
            }
        }

        return res;
    }
};



#endif //SEMANTICANALYSER_H
