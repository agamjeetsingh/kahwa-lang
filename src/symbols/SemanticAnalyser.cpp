//
// Created by Agamjeet Singh on 19/12/25.
//

#include "../../include/symbols/SemanticAnalyser.h"

ClassSymbol *SemanticAnalyser::declareClass(const ClassDecl *classDecl, Scope *scope) {
    auto classSymbol = astArena.make<ClassSymbol>(classDecl->name, scope);

    // ===== TypeParameterSymbols and Variances ======

    // Convert ClassDecl's TypeRefs to TypeParameterSymbols
    std::vector<std::pair<TypeParameterSymbol*, SourceRange>> typeParameters;
    typeParameters.reserve(classDecl->typeParameters.size());

    std::ranges::transform(
        classDecl->typeParameters,
        std::back_inserter(typeParameters),
        [](const std::pair<TypeRef*, Variance>& pair) {
            assert(pair.first->args.empty()); // Parser shouldn't allow any generic type parameters
            return std::pair{TypeParameterSymbolBuilder(pair.first->identifier, pair.second).build(), pair.first->bodyRange};
        }
    );

    // Register all the typeParameters to the scope, reporting duplicates to the diagnostic engine
    std::ranges::for_each(typeParameters, [classSymbol, this](const std::pair<TypeParameterSymbol*, SourceRange>& pair) {
       if (!classSymbol->scope.searchCurrentUnique(pair.first->name)) {
           diagnosticEngine.reportProblem(
               DiagnosticSeverity::ERROR,
               DiagnosticKind::SYMBOL_ALREADY_DECLARED,
               pair.second,
               toMsg(DiagnosticKind::SYMBOL_ALREADY_DECLARED, pair.first->name));
       } else {
           classSymbol->scope.define(pair.first);
           classSymbol->addGenericArgument(pair.first);
       }
    });

    // ===== Nested Classes =====

    // Convert ClassDecl's nestedClasses ClassDecl*s to ClassSymbol*s
    std::vector<std::pair<ClassSymbol*, SourceRange>> nestedClasses;
    nestedClasses.reserve(classDecl->nestedClasses.size());
    std::ranges::transform(classDecl->nestedClasses, std::back_inserter(nestedClasses), [this, classSymbol](const ClassDecl* classDecl) {
        return std::pair{declareClass(classDecl, &classSymbol->scope), classDecl->nameSourceRange};
    });

    // Register all the nested classes to the scope, reporting duplicates to the diagnostic engine
    std::ranges::for_each(nestedClasses, [classSymbol, this](const std::pair<ClassSymbol*, SourceRange>& pair) {
       if (!classSymbol->scope.searchCurrentUnique(pair.first->name)) {
           diagnosticEngine.reportProblem(
               DiagnosticSeverity::ERROR,
               DiagnosticKind::SYMBOL_ALREADY_DECLARED,
               pair.second,
               toMsg(DiagnosticKind::SYMBOL_ALREADY_DECLARED, pair.first->name));
       } else {
           classSymbol->scope.define(pair.first);
           classSymbol->addNestedClass(pair.first);
       }
    });

    // ===== Methods =====

    // Convert ClassDecl's MethodDecl*s to MethodSymbol*s
    std::vector<std::pair<MethodSymbol*, SourceRange>> methods;
    methods.reserve(classDecl->methods.size());
    std::ranges::transform(classDecl->methods, std::back_inserter(methods), [classSymbol, this](const MethodDecl* methodDecl) {
        return std::pair{declareMethod(methodDecl, &classSymbol->scope), methodDecl->nameSourceRange};
    });

    // Register all the methods to the scope, reporting duplicates to the diagnostic engine
    std::ranges::for_each(methods, [classSymbol, this](const std::pair<MethodSymbol*, SourceRange>& pair) {
        // Different methods with the same names are allowed because of method overloading
        classSymbol->scope.define(pair.first);
        classSymbol->addMethod(pair.first);
    });

    return classSymbol;
}

MethodSymbol *SemanticAnalyser::declareMethod(const MethodDecl *methodDecl, Scope *scope) const {
    auto methodSymbol = astArena.make<MethodSymbol>(methodDecl->name, scope);

    // ===== TypeParameterSymbols ======

    // Convert ClassDecl's TypeRefs to TypeParameterSymbols
    std::vector<std::pair<TypeParameterSymbol*, SourceRange>> typeParameters;
    typeParameters.reserve(methodDecl->typeParameters.size());

    std::ranges::transform(
        methodDecl->typeParameters,
        std::back_inserter(typeParameters),
        [](const TypeRef* typeParameter) {
            assert(typeParameter->args.empty()); // Parser shouldn't allow any generic type parameters
            return std::pair{TypeParameterSymbolBuilder(typeParameter->identifier).build(), typeParameter->bodyRange};
        }
    );

    // Register all the typeParameters to the scope, reporting duplicates to the diagnostic engine
    std::ranges::for_each(typeParameters, [methodSymbol, this](const std::pair<TypeParameterSymbol*, SourceRange>& pair) {
       if (!methodSymbol->scope.searchCurrentUnique(pair.first->name)) {
           diagnosticEngine.reportProblem(
               DiagnosticSeverity::ERROR,
               DiagnosticKind::SYMBOL_ALREADY_DECLARED,
               pair.second,
               toMsg(DiagnosticKind::SYMBOL_ALREADY_DECLARED, pair.first->name));
       } else {
           methodSymbol->scope.define(pair.first);
           methodSymbol->addGenericArgument(pair.first);
       }
    });

    return methodSymbol;
}

TranslationUnit *SemanticAnalyser::declareFile(const KahwaFile *kahwaFile) {
    // Ideally, all the symbols are in sorted order according to their position in the file
    auto* translationUnit = astArena.make<TranslationUnit>();

    // Convert the ClassDecl*s to ClassSymbol*s
    std::vector<std::pair<ClassSymbol*, SourceRange>> classes;
    classes.reserve(kahwaFile->classDecls.size());
    std::ranges::transform(kahwaFile->classDecls, std::back_inserter(classes), [this, translationUnit](const ClassDecl* classDecl) {
        return std::pair{declareClass(classDecl, &translationUnit->scope), classDecl->nameSourceRange};
    });

    // Register all the classes to the scope, reporting duplicates to the diagnostic engine
    std::ranges::for_each(classes, [translationUnit, this](const std::pair<ClassSymbol*, SourceRange>& pair) {
       if (!translationUnit->scope.searchCurrentUnique(pair.first->name)) {
           diagnosticEngine.reportProblem(
               DiagnosticSeverity::ERROR,
               DiagnosticKind::SYMBOL_ALREADY_DECLARED,
               pair.second,
               toMsg(DiagnosticKind::SYMBOL_ALREADY_DECLARED, pair.first->name));
       } else {
           translationUnit->scope.define(pair.first);
           translationUnit->classes.push_back(pair.first);
       }
    });

    return translationUnit;
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