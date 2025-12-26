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

ClassSymbol *SemanticAnalyser::declareClass(const ClassDecl *classDecl, Scope *scope) {
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

    return classSymbol;
}

template<typename FunctionLikeSymbol> requires std::is_same_v<FunctionLikeSymbol, FunctionSymbol> || std::is_same_v<FunctionLikeSymbol, MethodSymbol>
FunctionLikeSymbol *SemanticAnalyser::declareFunction(const MethodDecl *methodDecl, Scope *scope) {
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

    return methodSymbol;
}

TranslationUnit *SemanticAnalyser::declareFile(const KahwaFile *kahwaFile) {
    // Ideally, all the symbols are in sorted order according to their position in the file
    auto* translationUnit = astArena.make<TranslationUnit>();

    // ===== Classes =====
    registerIt<ClassSymbol>(translationUnit, kahwaFile->classDecls, [this, translationUnit](const ClassDecl* classDecl) {
        return std::pair{declareClass(classDecl, &translationUnit->scope), classDecl->nameSourceRange};
    }, [translationUnit](ClassSymbol* classSymbol){ translationUnit->addClass(classSymbol); });

    // ===== Functions =====
    registerIt<FunctionSymbol>(translationUnit, kahwaFile->functionDecls, [this, translationUnit](const MethodDecl* functionDecl) {
        return std::pair{declareFunction<FunctionSymbol>(functionDecl, &translationUnit->scope), functionDecl->nameSourceRange};
    }, [translationUnit](FunctionSymbol* functionSymbol){ translationUnit->addFunction(functionSymbol); }, true);

    // ===== Top level variables =====
    registerIt<VisibleVariableSymbol>(translationUnit, kahwaFile->variableDecls, [this, translationUnit](const FieldDecl* variableDecl) {
        return std::pair{declareVariable<VisibleVariableSymbol>(variableDecl, &translationUnit->scope), variableDecl->nameSourceRange};
    }, [translationUnit](VisibleVariableSymbol* variableSymbol){ translationUnit->addVariable(variableSymbol); });

    // Typedefs in Phase 2

    return translationUnit;
}

template<typename T> requires (std::is_same_v<T, VariableSymbol> || std::is_same_v<T, VisibleVariableSymbol> || std::is_same_v<T, FieldSymbol>)
T *SemanticAnalyser::declareVariable(const FieldDecl *variableDecl, Scope *scope) {
    return astArena.make<T>(variableDecl->name, scope);
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
        classSymbol->addSuperClass(analyseType(superClass, &classSymbol->scope));
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
    functionSymbol->returnType = analyseType(dynamic_cast<MethodDecl*>(symbolToDecl[functionSymbol])->returnType, functionSymbol->scope);

    std::ranges::for_each(functionSymbol->parameters, [this](VariableSymbol* variableSymbol) {
       resolveTypes(variableSymbol);
    });

    // TODO - Body
}

template<typename T> requires (std::is_same_v<T, VariableSymbol> || std::is_same_v<T, VisibleVariableSymbol> || std::is_same_v<T, FieldSymbol>)
void SemanticAnalyser::resolveTypes(T *variableSymbol) {
    variableSymbol->type = analyseType(dynamic_cast<FieldDecl*>(symbolToDecl[variableSymbol]->typeRef), variableSymbol->scope);
}


Modifier SemanticAnalyser::resolveModality(const std::vector<ModifierNode> &modalityModifiers) const {
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

Modifier SemanticAnalyser::resolveVisibility(const std::vector<ModifierNode> &visibilityModifiers, bool topLevel) const {
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