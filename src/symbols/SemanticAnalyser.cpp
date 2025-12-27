//
// Created by Agamjeet Singh on 19/12/25.
//

#include "../../include/symbols/SemanticAnalyser.h"
#include "../../include/symbols/Symbol.h"
#include "../../include/symbols/VisibleVariableSymbol.h"
#include "../../include/symbols/VariableSymbol.h"

template<typename ChildSymbol, typename ParentSymbol, typename DeclLike, typename F1, typename F2> requires std::derived_from<ChildSymbol, Symbol> && requires(ParentSymbol t) { { t.scope } -> std::same_as<Scope &>; } && std::invocable<F1, const DeclLike &> && std::same_as<std::invoke_result_t<F1, const DeclLike &>, std::pair<ChildSymbol *, SourceRange> > && std::invocable<F2, ChildSymbol *> && std::same_as<std::invoke_result_t<F2, ChildSymbol *>, void>
void SemanticAnalyser::registerIt(ParentSymbol *symbol, const std::vector<DeclLike> &decls, F1 &&declToSymbolAndSourceRange, F2 &&registerSymbol, bool duplicatesAllowed) {
    std::vector<std::tuple<ChildSymbol*, SourceRange, DeclLike>> ts;
    ts.reserve(decls.size());

    std::ranges::transform(decls, std::back_inserter(ts), [declToSymbolAndSourceRange](const DeclLike& declLike) {
        auto pair = declToSymbolAndSourceRange(declLike);
        return std::tuple<ChildSymbol*, SourceRange, DeclLike>{pair.first, pair.second, declLike};
    });

    std::ranges::for_each(ts, [this, &symbol, duplicatesAllowed, registerSymbol](const std::tuple<ChildSymbol*, SourceRange, DeclLike>& triple) {
        if (!duplicatesAllowed && !symbol->scope.searchCurrent(std::get<0>(triple)->name).empty()) {
            diagnosticEngine.reportProblem(
               DiagnosticSeverity::ERROR,
               DiagnosticKind::SYMBOL_ALREADY_DECLARED,
               std::get<1>(triple),
               toMsg(DiagnosticKind::SYMBOL_ALREADY_DECLARED, std::get<0>(triple)->name));
        } else {
            symbol->scope.define(std::get<0>(triple));
            registerSymbol(std::get<0>(triple));
            if constexpr (std::derived_from<std::remove_pointer_t<DeclLike>, Decl>) {
                symbolToDecl[std::get<0>(triple)] = std::get<2>(triple);
            }
        }
    });
}

ClassSymbol *SemanticAnalyser::declareClass(const ClassDecl *classDecl, Scope *scope, bool topLevel) {
    auto classSymbol = astArena.make<ClassSymbol>(classDecl->name, scope);

    // ===== TypeParameterSymbols and Variances ======

    registerIt<TypeParameterSymbol>(classSymbol, classDecl->typeParameters, [](const std::pair<TypeRef*, Variance>& pair) {
            assert(pair.first->args.empty()); // Parser shouldn't allow any generic type parameters
            return std::pair{TypeParameterSymbolBuilder(pair.first->identifier, pair.second).build(), pair.first->bodyRange};
        }, [classSymbol](TypeParameterSymbol* typeParameter) { classSymbol->addGenericArgument(typeParameter); });
    // ===== Nested Classes =====

    registerIt<ClassSymbol>(classSymbol, classDecl->nestedClasses, [this, classSymbol](const ClassDecl* classDecl) {
        return std::pair{declareClass(classDecl, &classSymbol->scope), classDecl->nameSourceRange};
    }, [classSymbol](ClassSymbol* nestedClass){ classSymbol->addNestedClass(nestedClass); });

    // ===== Methods =====

    // Different methods with the same names are allowed because of method overloading
    registerIt<MethodSymbol>(classSymbol, classDecl->methods, [classSymbol, this](const MethodDecl* methodDecl) {
        return std::pair{declareFunction<MethodSymbol>(methodDecl, &classSymbol->scope), methodDecl->nameSourceRange};
    }, [classSymbol](MethodSymbol* method){ classSymbol->addMethod(method); }, true);

    // ===== Fields =====

    registerIt<FieldSymbol>(classSymbol, classDecl->fields, [classSymbol, this](const FieldDecl* fieldDecl) {
       return std::pair{declareVariable<FieldSymbol>(fieldDecl, &classSymbol->scope), fieldDecl->nameSourceRange};
    }, [classSymbol](FieldSymbol* fieldSymbol){ classSymbol->addField(fieldSymbol); });

    // ===== Modifiers =====
    classSymbol->setVisibility(resolveVisibility(classDecl->modifiers, topLevel));

    // Static and Override keyword aren't allowed. Emitting diagnostics for each occurrence.
    modifierNotAllowed(classDecl->modifiers, Modifier::STATIC);
    modifierNotAllowed(classDecl->modifiers, Modifier::OVERRIDE);

    // Setting effective modality
    classSymbol->setModality(resolveModality(classDecl->modifiers));

    return classSymbol;
}

template<typename FunctionLikeSymbol> requires std::is_same_v<FunctionLikeSymbol, FunctionSymbol> || std::is_same_v<FunctionLikeSymbol, MethodSymbol>
FunctionLikeSymbol *SemanticAnalyser::declareFunction(const MethodDecl *methodDecl, Scope *scope, bool topLevel) {
    auto methodSymbol = astArena.make<FunctionLikeSymbol>(methodDecl->name, scope);

    // ===== TypeParameterSymbols ======

    registerIt<TypeParameterSymbol>(methodSymbol, methodDecl->typeParameters, [](const TypeRef* typeParameter) {
           assert(typeParameter->args.empty()); // Parser shouldn't allow any generic type parameters
           return std::pair{TypeParameterSymbolBuilder(typeParameter->identifier).build(), typeParameter->bodyRange};
       }, [methodSymbol](TypeParameterSymbol* typeParameter) { methodSymbol->addGenericArgument(typeParameter); });

    // ===== Function Parameters =====

    registerIt<VariableSymbol>(methodSymbol, methodDecl->parameters, [this, methodSymbol](const std::pair<TypeRef *, std::string>& pair) {
       return std::pair{astArena.make<VariableSymbol>(pair.second, &methodSymbol->scope), pair.first->nameSourceRange}; // TODO - Need source range of parameter name too
    }, [methodSymbol](VariableSymbol* variableSymbol){ methodSymbol->addParameter(variableSymbol); });

    // ===== Modifiers =====

    methodSymbol->setVisibility(resolveVisibility(methodDecl->modifiers, topLevel));
    methodSymbol->setStatic(hasModifier(methodDecl->modifiers, Modifier::STATIC));

    if constexpr (std::is_same_v<FunctionLikeSymbol, FunctionSymbol>) {
        assert(topLevel);

        modifierNotAllowed(methodDecl->modifiers, isModalityModifier);
        modifierNotAllowed(methodDecl->modifiers, Modifier::OVERRIDE);
    }
    if constexpr (std::is_same_v<FunctionLikeSymbol, MethodSymbol>) {
        assert(!topLevel);

        methodSymbol->setModality(resolveModality(methodDecl->modifiers));
        methodSymbol->setOverride(hasModifier(methodDecl->modifiers, Modifier::OVERRIDE));
    }

    return methodSymbol;
}

TranslationUnit *SemanticAnalyser::declareFile(const KahwaFile *kahwaFile) {
    // Ideally, all the symbols are in sorted order according to their position in the file
    auto* translationUnit = astArena.make<TranslationUnit>();

    // ===== Classes =====
    registerIt<ClassSymbol>(translationUnit, kahwaFile->classDecls, [this, translationUnit](const ClassDecl* classDecl) {
        return std::pair{declareClass(classDecl, &translationUnit->scope, true), classDecl->nameSourceRange};
    }, [translationUnit](ClassSymbol* classSymbol){ translationUnit->addClass(classSymbol); });

    // ===== Functions =====
    registerIt<FunctionSymbol>(translationUnit, kahwaFile->functionDecls, [this, translationUnit](const MethodDecl* functionDecl) {
        return std::pair{declareFunction<FunctionSymbol>(functionDecl, &translationUnit->scope, true), functionDecl->nameSourceRange};
    }, [translationUnit](FunctionSymbol* functionSymbol){ translationUnit->addFunction(functionSymbol); }, true);

    // ===== Top level variables =====
    registerIt<VisibleVariableSymbol>(translationUnit, kahwaFile->variableDecls, [this, translationUnit](const FieldDecl* variableDecl) {
        return std::pair{declareVariable<VisibleVariableSymbol>(variableDecl, &translationUnit->scope, true), variableDecl->nameSourceRange};
    }, [translationUnit](VisibleVariableSymbol* variableSymbol){ translationUnit->addVariable(variableSymbol); });

    // Typedefs in Phase 2

    return translationUnit;
}

template<typename T> requires (std::is_same_v<T, VariableSymbol> || std::is_same_v<T, VisibleVariableSymbol> || std::is_same_v<T, FieldSymbol>)
T *SemanticAnalyser::declareVariable(const FieldDecl *variableDecl, Scope *scope, bool topLevel) {
    auto variableSymbol = astArena.make<T>(variableDecl->name, scope);

    variableSymbol->setStatic(hasModifier(variableDecl->modifiers, Modifier::STATIC));

    if constexpr (std::is_same_v<T, VariableSymbol>) {
        assert(!topLevel);

        modifierNotAllowed(variableDecl->modifiers, [](Modifier modifier){ return modifier != Modifier::STATIC; });

        return variableSymbol;
    }

    variableSymbol->setVisibility(resolveVisibility(variableDecl->modifiers, topLevel));

    if constexpr (std::is_same_v<T, VisibleVariableSymbol>) {
        assert(topLevel);

        modifierNotAllowed(variableDecl->modifiers, isModalityModifier);
        modifierNotAllowed(variableDecl->modifiers, Modifier::OVERRIDE);

        return variableSymbol;
    }

    if constexpr (std::is_same_v<T, FieldSymbol>) {
        assert(!topLevel);
        variableSymbol->setModality(resolveModality(variableDecl->modifiers));
        variableSymbol->setOverride(hasModifier(variableDecl->modifiers, Modifier::OVERRIDE));
        return variableSymbol;
    }
}

void SemanticAnalyser::resolveTypes(TranslationUnit *translationUnit) {
    // TODO - typedefs

    std::ranges::for_each(translationUnit->classes, [this](ClassSymbol* classSymbol) {
       resolveTypes(classSymbol);
    });

    std::ranges::for_each(translationUnit->functions, [this](FunctionSymbol* functionSymbol) {
       resolveTypes(functionSymbol);
    });

    std::ranges::for_each(translationUnit->variables, [this](VisibleVariableSymbol* variableSymbol) {
        resolveTypes(variableSymbol);
    });
}

void SemanticAnalyser::resolveTypes(ClassSymbol *classSymbol) {
    std::ranges::for_each(dynamic_cast<ClassDecl*>(symbolToDecl[classSymbol])->superClasses, [this, classSymbol](TypeRef* superClass) {
        classSymbol->addSuperClass(resolveType(superClass, &classSymbol->scope));
    });

    std::ranges::for_each(classSymbol->nestedClasses, [this](ClassSymbol* nestedClass) {
       resolveTypes(nestedClass);
    });

    std::ranges::for_each(classSymbol->fields, [this](FieldSymbol* field) {
       resolveTypes(field);
    });

    std::ranges::for_each(classSymbol->methods, [this](MethodSymbol* method) {
       resolveTypes(method);
    });
}

template<typename FunctionLikeSymbol> requires std::is_same_v<FunctionLikeSymbol, FunctionSymbol> || std::is_same_v<FunctionLikeSymbol, MethodSymbol>
void SemanticAnalyser::resolveTypes(FunctionLikeSymbol *functionSymbol) {
    functionSymbol->returnType = resolveType(dynamic_cast<MethodDecl*>(symbolToDecl[functionSymbol])->returnType, &functionSymbol->scope);

    std::ranges::for_each(functionSymbol->parameters, [this](VariableSymbol* variableSymbol) {
       resolveTypes(variableSymbol);
    });

    // TODO - Body
}

template<typename T> requires (std::is_same_v<T, VariableSymbol> || std::is_same_v<T, VisibleVariableSymbol> || std::is_same_v<T, FieldSymbol>)
void SemanticAnalyser::resolveTypes(T *variableSymbol) {
    variableSymbol->type = resolveType(dynamic_cast<FieldDecl*>(symbolToDecl[variableSymbol])->typeRef, &variableSymbol->scope);
}

Type *SemanticAnalyser::resolveType(const TypeRef *typeRef, Scope *scope) {
    const auto& symbols = scope->search(typeRef->identifier);
    if (symbols.empty()) {
        diagnosticEngine.reportProblem(
            DiagnosticSeverity::ERROR,
            DiagnosticKind::CANNOT_RESOLVE_SYMBOL,
            typeRef->bodyRange,
            toMsg(DiagnosticKind::CANNOT_RESOLVE_SYMBOL, typeRef->identifier));
        return nullptr;
    }
    auto* symbol = symbols[0];
    auto typeSymbol = dynamic_cast<TypeSymbol*>(symbol);
    if (!typeSymbol) {
        diagnosticEngine.reportProblem(
            DiagnosticSeverity::ERROR,
            DiagnosticKind::SYMBOL_MISMATCH,
            typeRef->bodyRange,
            toMsg(DiagnosticKind::SYMBOL_MISMATCH, Symbol::toString(symbol), "type"));
        return nullptr;
    }

    auto builder = TypeBuilder(typeSymbol);

    std::size_t expectedGenericsArgs = 0;
    if (auto classSymbol = dynamic_cast<ClassSymbol*>(typeSymbol)) {
        expectedGenericsArgs = classSymbol->genericArguments.size();
    }

    if (expectedGenericsArgs != typeRef->args.size()) {
        diagnosticEngine.reportProblem(
            DiagnosticSeverity::ERROR,
            DiagnosticKind::INCORRECT_NUMBER_OF_TYPE_PARAMETERS,
            typeRef->bodyRange,
            toMsg(DiagnosticKind::INCORRECT_NUMBER_OF_TYPE_PARAMETERS, expectedGenericsArgs, Symbol::toString(symbol), typeRef->args.size()));
        return nullptr;
    }

    for (auto arg: typeRef->args) {
        if (auto argType = resolveType(arg, scope)) {
            builder.with(argType);
        } else {
            return nullptr;
        }
    }

    return builder.build();
}


Modifier SemanticAnalyser::resolveModality(const std::vector<ModifierNode>& modifiers) const {
    auto modalityModifiersView = modifiers | std::ranges::views::filter([](const ModifierNode& modifier) {
        return isModalityModifier(modifier.modifier);
    });

    std::vector<ModifierNode> modalityModifiers = std::vector(modalityModifiersView.begin(), modalityModifiersView.end());

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

Modifier SemanticAnalyser::resolveVisibility(const std::vector<ModifierNode>& modifiers, bool topLevel) const {
    auto visibilityModifiersView = modifiers | std::ranges::views::filter([](const ModifierNode& modifier) {
        return isVisibilityModifier(modifier.modifier);
    });

    auto visibilityModifiers = std::vector(visibilityModifiersView.begin(), visibilityModifiersView.end());

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

bool SemanticAnalyser::hasModifier(const std::vector<ModifierNode> &modifiers, Modifier modifier) const {
    bool found = false;
    for (auto desiredModifier : modifiers |
        std::ranges::views::filter([modifier](const ModifierNode& modifierNode) {
            return modifierNode.modifier == modifier;
        })) {
        if (!found) {
            found = true;
        } else {
            diagnosticEngine.reportProblem(
            DiagnosticSeverity::ERROR,
            DiagnosticKind::REPEATED_MODIFIER,
            desiredModifier.sourceRange,
            toMsg(DiagnosticKind::MODIFIER_NOT_ALLOWED, desiredModifier.modifier));
        }
        }
    return found;
}


void SemanticAnalyser::modifierNotAllowed(const std::vector<ModifierNode> &modifiers, Modifier notAllowed) const {
    modifierNotAllowed(modifiers, [notAllowed](Modifier modifier){ return modifier == notAllowed; });
}

void SemanticAnalyser::modifierNotAllowed(const std::vector<ModifierNode> &modifiers, std::function<bool(Modifier)> pred) const {
    for (auto notAllowedModifier : modifiers |
        std::ranges::views::filter([pred](const ModifierNode& modifier) {
            return pred(modifier.modifier);
        })) {
        diagnosticEngine.reportProblem(
            DiagnosticSeverity::ERROR,
            DiagnosticKind::MODIFIER_NOT_ALLOWED,
            notAllowedModifier.sourceRange,
            toMsg(DiagnosticKind::MODIFIER_NOT_ALLOWED, notAllowedModifier.modifier));
        }
}
