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
#include "VisibleVariableSymbol.h"

class DiagnosticEngine;

/**
 * Building symbol table:
 * Phase 1: declareFile, which declares all symbols so they can be accessed
 * Phase 2: analyseFile, which resolves all symbols, including the ones in method bodies. This step also adds all modifiers
 * Phase 3: Rest of the semantics (like checking subtyping rules, inheritance rules, trying to use a private method etc.)
 */
class SemanticAnalyser {
public:
    explicit SemanticAnalyser(Arena& astArena, DiagnosticEngine& diagnosticEngine):
    astArena(astArena),
    diagnosticEngine(diagnosticEngine) {
        SymbolBuilder::setArena(&astArena);
    }

    ClassSymbol* declareClass(const ClassDecl* classDecl, Scope* scope);

    template<typename FunctionLikeSymbol>
    requires std::is_same_v<FunctionLikeSymbol, FunctionSymbol> || std::is_same_v<FunctionLikeSymbol, MethodSymbol>
    FunctionLikeSymbol* declareFunction(const MethodDecl* methodDecl, Scope* scope);

    template<typename ChildSymbol, typename ParentSymbol, typename DeclLike, typename F1, typename F2>
    requires std::derived_from<ChildSymbol, Symbol> && requires(ParentSymbol t) {
        { t.scope } -> std::same_as<Scope&>;  // Ensure scope is a field
    } &&
                std::invocable<F1, const DeclLike&> && std::same_as<std::invoke_result_t<F1, const DeclLike&>, std::pair<ChildSymbol*, SourceRange>> &&
                    std::invocable<F2, ChildSymbol*> && std::same_as<std::invoke_result_t<F2, ChildSymbol*>, void>
    void registerIt(
        ParentSymbol* symbol,
        const std::vector<DeclLike>& decls,
        F1&& declToSymbolAndSourceRange,
        F2&& registerSymbol,
        bool duplicatesAllowed = false) const {
        std::vector<std::pair<ChildSymbol*, SourceRange>> ts;
        ts.reserve(decls.size());

        std::ranges::transform(decls, std::back_inserter(ts), declToSymbolAndSourceRange);

        std::ranges::for_each(ts, [this, &symbol, duplicatesAllowed, registerSymbol](const std::pair<ChildSymbol*, SourceRange>& pair) {
            if (!duplicatesAllowed && !symbol->scope.searchCurrentUnique(pair.first->name)) {
                diagnosticEngine.reportProblem(
                   DiagnosticSeverity::ERROR,
                   DiagnosticKind::SYMBOL_ALREADY_DECLARED,
                   pair.second,
                   toMsg(DiagnosticKind::SYMBOL_ALREADY_DECLARED, pair.first->name));
            } else {
                symbol->scope.define(pair.first);
                registerSymbol(pair.first);
            }
        });
    }

    template <typename T>
    requires (std::is_same_v<T, VariableSymbol> || std::is_same_v<T, VisibleVariableSymbol> || std::is_same_v<T, FieldSymbol>)
    T* declareVariable(const FieldDecl* variableDecl, Scope* scope);

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
        const auto& symbol = scope->searchUnique(typeRef->identifier);
        if (!symbol) {
            // TODO - Send some diagnostics
            return nullptr;
        }
        auto typeSymbol = dynamic_cast<TypeSymbol*>(symbol.value());
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
