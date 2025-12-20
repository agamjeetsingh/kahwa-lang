//
// Created by Agamjeet Singh on 19/12/25.
//

#include <gtest/gtest.h>

#include "../DiagnosticTesting.h"
#include "../../include/arena/Arena.h"
#include "../../include/diagnostics/DiagnosticEngine.h"
#include "../../include/symbols/SemanticAnalyser.h"

class SemanticAnalyserTest : public testing::Test, public DiagnosticTesting {
protected:
    inline static Arena astArena;
    SemanticAnalyser semantic_analyser{astArena, diagnostic_engine};

    void SetUp() override {
        ASTBuilder::setArena(&astArena);
    }

    static bool classSymbolEqualIgnoreSourceRange(const ClassSymbol* cs1, const ClassSymbol* cs2) {
        if (cs1 == nullptr && cs2 == nullptr) return true;
        if (cs1 == nullptr || cs2 == nullptr) return false;
        
        if (cs1->name != cs2->name ||
            cs1->isAbstract != cs2->isAbstract ||
            cs1->isOpen != cs2->isOpen ||
            cs1->visibility != cs2->visibility) {
            return false;
        }
        
        if (cs1->genericArguments.size() != cs2->genericArguments.size() ||
            cs1->superClasses.size() != cs2->superClasses.size() ||
            cs1->methods.size() != cs2->methods.size() ||
            cs1->nestedClasses.size() != cs2->nestedClasses.size()) {
            return false;
        }
        
        for (size_t i = 0; i < cs1->genericArguments.size(); ++i) {
            if (!typeParameterSymbolEqualIgnoreSourceRange(cs1->genericArguments[i], cs2->genericArguments[i])) return false;
        }
        
        for (size_t i = 0; i < cs1->superClasses.size(); ++i) {
            if (!classSymbolEqualIgnoreSourceRange(cs1->superClasses[i], cs2->superClasses[i])) return false;
        }
        
        for (size_t i = 0; i < cs1->methods.size(); ++i) {
            if (!methodSymbolEqualIgnoreSourceRange(cs1->methods[i], cs2->methods[i])) return false;
        }
        
        for (size_t i = 0; i < cs1->nestedClasses.size(); ++i) {
            if (!classSymbolEqualIgnoreSourceRange(cs1->nestedClasses[i], cs2->nestedClasses[i])) return false;
        }
        
        return true;
    }

    static bool methodSymbolEqualIgnoreSourceRange(const MethodSymbol* ms1, const MethodSymbol* ms2) {
        // TODO: Implement when MethodSymbol structure is defined
        if (ms1 == nullptr && ms2 == nullptr) return true;
        if (ms1 == nullptr || ms2 == nullptr) return false;
        return true; // Stub implementation
    }

    static bool typeParameterSymbolEqualIgnoreSourceRange(const TypeParameterSymbol* ts1, const TypeParameterSymbol* ts2) {
        // TODO: Implement when TypeParameterSymbol structure is defined
        if (ts1 == nullptr && ts2 == nullptr) return true;
        if (ts1 == nullptr || ts2 == nullptr) return false;
        return true; // Stub implementation
    }

    const std::vector<Modifier> illegalModifiersForClass = {Modifier::STATIC, Modifier::PROTECTED};
    const std::vector<Modifier> illegalModifiersForNestedClass = {Modifier::STATIC};
    const std::vector<Modifier> legalModifiersForClass = {Modifier::OPEN, Modifier::FINAL, Modifier::ABSTRACT, Modifier::PUBLIC, Modifier::PRIVATE};
    const std::vector<Modifier> legalModifiersForNestedClass = {Modifier::OPEN, Modifier::FINAL, Modifier::ABSTRACT, Modifier::PUBLIC, Modifier::PRIVATE, Modifier::PROTECTED};
};

TEST_F(SemanticAnalyserTest, ReportsProblemsForIllegalModifiersOnClass) {
    for (auto modifier: illegalModifiersForClass) {
        EXPECT_PRED2(classSymbolEqualIgnoreSourceRange, ClassSymbolBuilder("myClass").build(), semantic_analyser.analyseClass(
            ClassDeclBuilder("myClass").with(modifier).build()));

        expectErrorsIgnoreSourceRange({{DiagnosticKind::MODIFIER_NOT_ALLOWED, toMsg(DiagnosticKind::MODIFIER_NOT_ALLOWED, modifier)}});
    }

    for (auto modifier: illegalModifiersForNestedClass) {
        EXPECT_PRED2(classSymbolEqualIgnoreSourceRange,
            ClassSymbolBuilder("myClass").withNestedClass(ClassSymbolBuilder("nestedClass").setVisibility(Modifier::PRIVATE).build()).build(),
            semantic_analyser.analyseClass(ClassDeclBuilder("myClass")
                .with(ClassDeclBuilder("nestedClass")
                    .with(modifier)
                    .build())
                .build()));

        expectErrorsIgnoreSourceRange({{DiagnosticKind::MODIFIER_NOT_ALLOWED, toMsg(DiagnosticKind::MODIFIER_NOT_ALLOWED, modifier)}});
    }

    for (auto modifier: legalModifiersForClass) {
        EXPECT_PRED2(classSymbolEqualIgnoreSourceRange, ClassSymbolBuilder("myClass").with(modifier).build(), semantic_analyser.analyseClass(
            ClassDeclBuilder("myClass").with(modifier).build()));

        expectNoDiagnostics();
    }

    for (auto modifier: legalModifiersForNestedClass) {
        EXPECT_PRED2(classSymbolEqualIgnoreSourceRange,
            ClassSymbolBuilder("myClass")
            .withNestedClass(ClassSymbolBuilder("nestedClass")
                .setVisibility(Modifier::PRIVATE)
                .with(modifier)
                .build())
            .build(),
            semantic_analyser.analyseClass(
                ClassDeclBuilder("myClass")
                .with(ClassDeclBuilder("nestedClass")
                    .with(modifier)
                    .build())
                .build()));

        expectNoDiagnostics();
    }
}

TEST_F(SemanticAnalyserTest, ReportsProblemForIllegalModifierCombinationsWithTwoModifiersOnClass) {

    std::vector<std::vector<Modifier>> illegalCombinations = {
        {Modifier::ABSTRACT, Modifier::FINAL},
        {Modifier::FINAL, Modifier::ABSTRACT},
        {Modifier::PRIVATE, Modifier::PUBLIC},
        {Modifier::PUBLIC, Modifier::PRIVATE},
        {Modifier::FINAL, Modifier::OPEN},
        {Modifier::OPEN, Modifier::FINAL},
    };

    std::vector effectiveModifiers = {
        Modifier::ABSTRACT,
        Modifier::ABSTRACT,
        Modifier::PRIVATE,
        Modifier::PUBLIC,
        Modifier::OPEN,
        Modifier::OPEN
    };

    for (int i = 0; i < illegalCombinations.size(); i++) {
        const auto& illegalCombination = illegalCombinations[i];
        EXPECT_PRED2(classSymbolEqualIgnoreSourceRange, ClassSymbolBuilder("myClass").with(effectiveModifiers[i]).build(), semantic_analyser.analyseClass(
            ClassDeclBuilder("myClass").with(illegalCombination).build()));

        expectErrorsIgnoreSourceRange({{
            DiagnosticKind::ILLEGAL_MODIFIER_COMBINATION,
            toMsg(DiagnosticKind::ILLEGAL_MODIFIER_COMBINATION,
                illegalCombination[0],
                illegalCombination[1])
            }});
    }
}

TEST_F(SemanticAnalyserTest, ReportsProblemForRepeatedModifiersOnClass) {

    std::vector<std::vector<Modifier>> repeatedModifiers = {
        {Modifier::PUBLIC, Modifier::PUBLIC},
        {Modifier::PRIVATE, Modifier::PRIVATE},
        {Modifier::ABSTRACT, Modifier::ABSTRACT},
        {Modifier::OPEN, Modifier::OPEN},
        {Modifier::FINAL, Modifier::FINAL}
    };

    for (auto& repeatedModifierPair: repeatedModifiers) {
        EXPECT_PRED2(classSymbolEqualIgnoreSourceRange, ClassSymbolBuilder("myClass").with(repeatedModifierPair[0]).build(), semantic_analyser.analyseClass(
            ClassDeclBuilder("myClass").with(repeatedModifierPair).build()));

        expectErrorsIgnoreSourceRange({{
            DiagnosticKind::REPEATED_MODIFIER,
            toMsg(DiagnosticKind::REPEATED_MODIFIER, repeatedModifierPair[0])
            }});
    }
}

TEST_F(SemanticAnalyserTest, DifferentModifierErrorsDoNotInterfereWithEachOther) {
    EXPECT_PRED2(classSymbolEqualIgnoreSourceRange,
        ClassSymbolBuilder("myClass").setModality(Modifier::ABSTRACT).setVisibility(Modifier::PRIVATE).build(),
        semantic_analyser.analyseClass(
            ClassDeclBuilder("myClass")
            .with({Modifier::PRIVATE, Modifier::ABSTRACT, Modifier::PRIVATE})
            .build()));

    expectErrorsIgnoreSourceRange({{
            DiagnosticKind::REPEATED_MODIFIER, toMsg(DiagnosticKind::REPEATED_MODIFIER, Modifier::PRIVATE)
        }});

    EXPECT_PRED2(classSymbolEqualIgnoreSourceRange,
        ClassSymbolBuilder("myClass").setVisibility(Modifier::PRIVATE).setModality(Modifier::ABSTRACT).build(),
        semantic_analyser.analyseClass(
            ClassDeclBuilder("myClass")
            .with({Modifier::FINAL, Modifier::ABSTRACT, Modifier::PRIVATE})
            .build()));

    expectErrorsIgnoreSourceRange({{
            DiagnosticKind::ILLEGAL_MODIFIER_COMBINATION, toMsg(DiagnosticKind::ILLEGAL_MODIFIER_COMBINATION, Modifier::FINAL, Modifier::ABSTRACT)
        }});
}
