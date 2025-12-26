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

    // ===== Phase 1 =====
    // TODO - Method Bodies

    ClassSymbol* declareClass(const ClassDecl* classDecl, Scope* scope, bool topLevel = false);

    template<typename FunctionLikeSymbol>
    requires std::is_same_v<FunctionLikeSymbol, FunctionSymbol> || std::is_same_v<FunctionLikeSymbol, MethodSymbol>
    FunctionLikeSymbol* declareFunction(const MethodDecl* methodDecl, Scope* scope, bool topLevel = false);

    template <typename T>
    requires (std::is_same_v<T, VariableSymbol> || std::is_same_v<T, VisibleVariableSymbol> || std::is_same_v<T, FieldSymbol>)
    T* declareVariable(const FieldDecl* variableDecl, Scope* scope, bool topLevel = false);

    TranslationUnit* declareFile(const KahwaFile* kahwaFile);

    // ===== Phase 2 =====

    void resolveTypes(TranslationUnit* translationUnit);

    void resolveTypes(ClassSymbol* classSymbol);

    template<typename FunctionLikeSymbol>
    requires std::is_same_v<FunctionLikeSymbol, FunctionSymbol> || std::is_same_v<FunctionLikeSymbol, MethodSymbol>
    void resolveTypes(FunctionLikeSymbol* functionSymbol);

    template <typename T>
    requires (std::is_same_v<T, VariableSymbol> || std::is_same_v<T, VisibleVariableSymbol> || std::is_same_v<T, FieldSymbol>)
    void resolveTypes(T* variableSymbol);

    std::unordered_map<Symbol*, Decl*> symbolToDecl;

    // ===== Phase 3 =====

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

    [[nodiscard]] Modifier resolveModality(const std::vector<ModifierNode>& modifiers) const;

    [[nodiscard]] Modifier resolveVisibility(const std::vector<ModifierNode>& modifiers, bool topLevel) const;

    [[nodiscard]] bool hasModifier(const std::vector<ModifierNode>& modifiers, Modifier modifier) const;

    template<typename ChildSymbol, typename ParentSymbol, typename DeclLike, typename F1, typename F2>
    requires std::derived_from<ChildSymbol, Symbol> && requires(ParentSymbol t) {
        { t.scope } -> std::same_as<Scope&>;  // Ensure scope is a field
    } && std::invocable<F1, const DeclLike&> &&
         std::same_as<std::invoke_result_t<F1, const DeclLike&>, std::pair<ChildSymbol*, SourceRange>> &&
         std::invocable<F2, ChildSymbol*> && std::same_as<std::invoke_result_t<F2, ChildSymbol*>, void>
    void registerIt(
        ParentSymbol* symbol,
        const std::vector<DeclLike>& decls,
        F1&& declToSymbolAndSourceRange,
        F2&& registerSymbol,
        bool duplicatesAllowed = false);

    void modifierNotAllowed(const std::vector<ModifierNode>& modifiers, Modifier notAllowed) const;

    void modifierNotAllowed(const std::vector<ModifierNode>& modifiers, std::function<bool(Modifier)> pred) const;
};



#endif //SEMANTICANALYSER_H
