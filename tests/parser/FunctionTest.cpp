//
// Created by Agamjeet Singh on 04/12/25.
//

#include <gtest/gtest.h>

#include "../DiagnosticTesting.h"
#include "../../include/diagnostics/DiagnosticEngine.h"
#include "../../include/parser/Parser.h"
#include "../../include/tokeniser/Tokeniser.h"
#include "../../include/parser/Modifier.h"
#include "StmtTestFixture.h"
#include "../../include/symbols/TypeParameterSymbol.h"

class FunctionTest : public StmtTestFixture {
protected:
    void testFunctions(const std::vector<std::pair<std::string, MethodDecl*>>& tests) const {
        for (auto& [str, expectedMethod]: tests) {
            auto parsedMethod = parseMethod(str);
            EXPECT_PRED2(methodDeclEqualIgnoreSourceRange, parsedMethod, expectedMethod);
            std::cout << "parsedMethod: " << toString(parsedMethod) << "\nexpectedMethod: " << toString(expectedMethod) << "\n--------" << std::endl;
        }
    }

    void testFunctions(const std::vector<MethodDecl*>& methods) const {
        for (auto* expectedMethod: methods) {
            auto parsedMethod = parseMethod(toString(expectedMethod));
            EXPECT_PRED2(methodDeclEqualIgnoreSourceRange, parsedMethod, expectedMethod);
            std::cout << "parsedMethod: " << toString(parsedMethod) << "\nexpectedMethod: " << toString(expectedMethod) << "\n--------" << std::endl;
        }
    }

    void testClasses(const std::vector<ClassDecl*>& classes) const {
        for (auto* expectedClass: classes) {
            auto parsedClass = parseClass(toString(expectedClass));
            EXPECT_PRED2(classDeclEqualIgnoreSourceRange, parsedClass, expectedClass);
            std::cout << "parsedClass: " << toString(parsedClass) << "\nexpectedClass: " << toString(expectedClass) << "\n--------" << std::endl;
        }
    }
};

TEST_F(FunctionTest, ParsesTopLevelMethodsCorrectly) {
    std::vector<MethodDecl*> functions;

    std::vector modifiers = {Modifier::PRIVATE, Modifier::STATIC};
    std::vector<std::pair<TypeRef*, std::string>> params = {
        {TypeRefBuilder("int").build(), "length"},
        {TypeRefBuilder("vector") // vector<int>
            .with(TypeRefBuilder("int").build())
            .build(), "list"}
    };
    std::vector<Expr*> defaultExprs = {nullptr};
    defaultExprs.insert(defaultExprs.end(), exampleExprs.begin(), exampleExprs.end());

    for (int i = 0; i <= modifiers.size(); i++) {
        for (int j = 0; j <= params.size(); j++) {
            for (auto body: exampleBlocks) {
                for (auto defaultExpr: defaultExprs) {
                    std::vector<Modifier> modifierSubset{modifiers.begin(), modifiers.begin() + i};
                    std::vector<FieldDecl*> paramSubset;
                    std::transform(params.begin(), params.begin() + j, std::back_inserter(paramSubset), [defaultExpr](const std::pair<TypeRef*, std::string>& pair) {
                        return FieldDeclBuilder(pair.second, pair.first).with(defaultExpr).build();
                    });

                    functions.push_back(MethodDeclBuilder("foo", TypeRefBuilder("int").build(),
                        body)
                        .with(modifierSubset)
                        .with(paramSubset)
                        .build());
                }
            }
        }
    }

    testFunctions(functions);

    expectNoDiagnostics();
}

TEST_F(FunctionTest, ParsesClassMethodsCorrectly) {
    std::vector<MethodDecl*> functions;

    std::vector modifiers = {Modifier::PRIVATE, Modifier::STATIC};
    std::vector<std::pair<TypeRef*, std::string>> params = {
        {TypeRefBuilder("int").build(), "length"},
        {TypeRefBuilder("vector") // vector<int>
            .with(TypeRefBuilder("int").build())
            .build(), "list"}
    };
    std::vector<Expr*> defaultExprs = {nullptr};
    defaultExprs.insert(defaultExprs.end(), exampleExprs.begin(), exampleExprs.end());

    for (int i = 0; i <= modifiers.size(); i++) {
        for (int j = 0; j <= params.size(); j++) {
            for (auto body: exampleBlocks) {
                for (auto defaultExpr: defaultExprs) {
                    std::vector<Modifier> modifierSubset{modifiers.begin(), modifiers.begin() + i};
                    std::vector<FieldDecl*> paramSubset;
                    std::transform(params.begin(), params.begin() + j, std::back_inserter(paramSubset), [defaultExpr](const std::pair<TypeRef*, std::string>& pair) {
                        return FieldDeclBuilder(pair.second, pair.first).with(defaultExpr).build();
                    });

                    functions.push_back(MethodDeclBuilder("foo", TypeRefBuilder("int").build(),
                        body)
                        .with(modifierSubset)
                        .with(paramSubset)
                        .build());
                }
            }
        }
    }

    std::vector<ClassDecl*> classes;

    for (int i = 0; i < std::min(functions.size(), static_cast<std::size_t>(300)); i++) {
        classes.push_back(ClassDeclBuilder("bar").with(std::vector(functions.begin(), functions.begin() + i + 1)).build());
    }

    testClasses(classes);

    expectNoDiagnostics();
}

// TODO - Check Generics

TEST_F(FunctionTest, ParsesFunctionsWithGenericsCorrectly) {
    std::vector<MethodDecl*> functions;

    std::vector modifiers = {Modifier::PRIVATE, Modifier::STATIC};
    std::vector<std::pair<TypeRef*, std::string>> params = {
        {TypeRefBuilder("int").build(), "length"},
        {TypeRefBuilder("vector") // vector<int>
            .with(TypeRefBuilder("int").build())
            .build(), "list"}
    };
    std::vector<Expr*> defaultExprs = {nullptr};
    defaultExprs.insert(defaultExprs.end(), exampleExprs.begin(), exampleExprs.end());

    std::vector genericParameters = {
        astArena.make<TypeParameterDecl>(dummy_source, "T", Variance::INVARIANT),
        astArena.make<TypeParameterDecl>(dummy_source, "U", Variance::INVARIANT),
        astArena.make<TypeParameterDecl>(dummy_source, "V", Variance::INVARIANT)
    };

    for (int i = 0; i <= modifiers.size(); i++) {
        for (int j = 0; j <= params.size(); j++) {
            for (auto body: exampleBlocks) {
                for (auto defaultExpr: defaultExprs) {
                    for (int k = 0; k <= genericParameters.size(); k++) {
                        std::vector<Modifier> modifierSubset{modifiers.begin(), modifiers.begin() + i};
                        std::vector<FieldDecl*> paramSubset;
                        std::transform(params.begin(), params.begin() + j, std::back_inserter(paramSubset), [defaultExpr](const std::pair<TypeRef*, std::string>& pair) {
                            return FieldDeclBuilder(pair.second, pair.first).with(defaultExpr).build();
                        });
                        std::vector<TypeParameterDecl*> genericParameterSubset{genericParameters.begin(), genericParameters.begin() + k};
                        functions.push_back(MethodDeclBuilder("foo", TypeRefBuilder("int").build(),
                            body)
                            .with(modifierSubset)
                            .with(paramSubset)
                            .with(genericParameterSubset)
                            .build());
                    }
                }
            }
        }
    }

    testFunctions(functions);

    expectNoDiagnostics();
}