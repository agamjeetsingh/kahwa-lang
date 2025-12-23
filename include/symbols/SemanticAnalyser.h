//
// Created by Agamjeet Singh on 19/12/25.
//

#ifndef SEMANTICANALYSER_H
#define SEMANTICANALYSER_H
#include <ranges>

#include "../parser/ClassDecl.h"
#include "../diagnostics/DiagnosticEngine.h"
#include "ClassSymbol.h"
#include "TranslationUnit.h"
#include "../parser/KahwaFile.h"
#include "../types/Type.h"

class DiagnosticEngine;

class SemanticAnalyser {
public:
    explicit SemanticAnalyser(Arena& astArena, DiagnosticEngine& diagnosticEngine):
    astArena(astArena),
    diagnosticEngine(diagnosticEngine) {
        SymbolBuilder::setArena(&astArena);
    }

    ClassSymbol* declareClass(const ClassDecl* classDecl, Scope* scope);

    MethodSymbol* declareMethod(const MethodDecl* methodDecl, Scope* scope) const;

    TranslationUnit* declareFile(const KahwaFile* kahwaFile);

    ClassSymbol* analyseClass(const ClassDecl* classDecl, Scope* scope, bool topLevel = true) {
        auto classSymbol = astArena.make<ClassSymbol>(classDecl->name, scope);

        // Actually first I might need to add all symbols onto my classSymbol's scope so that they can reference each other...

        return classSymbol; // TODO
        // Adding all methods
        std::vector<MethodSymbol*> methods{classDecl->methods.size()};
        std::ranges::transform(classDecl->methods, methods.begin(), [this, &classSymbol](const MethodDecl* methodDecl) {
            return analyseMethod(methodDecl, &classSymbol->scope);
        });
        // builder.with(methods);

        // Adding all nested classes (recursively)
        std::vector<ClassSymbol*> nestedClasses{classDecl->nestedClasses.size()};
        // std::ranges::transform(classDecl->nestedClasses, nestedClasses.begin(), [this](const ClassDecl* nestedClass) {
        //     return analyseClass(nestedClass, false);
        // });
        // builder.withNestedClasses(nestedClasses);

        // Setting effective visibility
        auto visibilityModifiers = classDecl->modifiers | std::ranges::views::filter([](const ModifierNode& modifier) {
            return isVisibilityModifier(modifier.modifier);
        });
        // builder.setVisibility(resolveVisibility(std::vector(visibilityModifiers.begin(), visibilityModifiers.end()), topLevel));

        // Static keyword isn't allowed. Emitting diagnostics for each occurrence.
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
        // builder.setModality(resolveModality(std::vector(modalityModifiers.begin(), modalityModifiers.end())));

        // return builder.build();
    }


    MethodSymbol* analyseMethod(const MethodDecl* methodDecl, Scope* scope) {


        return nullptr; // TODO
    }

    Type* analyseType(const TypeRef* typeRef, Scope* scope) {
        auto* symbol = scope->search(typeRef->identifier);
        if (!symbol) {
            // TODO - Send some diagnostics
            return nullptr;
        }
        auto typeSymbol = dynamic_cast<TypeSymbol*>(symbol);
        if (!typeSymbol) {
            // TODO - Send some diagnostics
            return nullptr;
        }

        auto builder = TypeBuilder(typeSymbol);

        std::size_t expectedGenericsArgs = 0;
        if (auto classSymbol = dynamic_cast<ClassSymbol*>(typeSymbol)) {
            expectedGenericsArgs = classSymbol->genericArguments.size();
        }

        if (expectedGenericsArgs != typeRef->args.size()) {
            // TODO - Send some diagnostics
            return nullptr;
        }

        for (auto arg: typeRef->args) {
            if (auto argType = analyseType(arg, scope)) {
                builder.with(argType);
            } else {
                // TODO - Send some diagnostics
                return nullptr;
            }
        }

        return builder.build();
    }

private:
    Arena& astArena;
    DiagnosticEngine& diagnosticEngine;

    [[nodiscard]] Modifier resolveModality(const std::vector<ModifierNode> &modalityModifiers) const;

    [[nodiscard]] Modifier resolveVisibility(const std::vector<ModifierNode> &visibilityModifiers, bool topLevel) const;
};



#endif //SEMANTICANALYSER_H
