//
// Created by Agamjeet Singh on 28/12/25.
//

#include <gtest/gtest.h>
#include "ParserTestBase.cpp"

class TypedefTest : public testing::Test, public ParserTestBase {
    
};

TEST_F(TypedefTest, ParsesSingleTypedefCorrectly) {
    const auto typedefDecl1 = TypedefDeclBuilder("myInt", TypeRefBuilder("int").build()).build();
    const auto str1 = toString(typedefDecl1);
    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(str1), KahwaFileBuilder().with(typedefDecl1).build());

    const auto typedefDecl2 = TypedefDeclBuilder("SomeType", TypeRefBuilder("someOtherType").build()).with(Modifier::PRIVATE).build();
    const auto str2 = toString(typedefDecl2);
    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(str2), KahwaFileBuilder().with(typedefDecl2).build());

    const auto typedefDecl3 = TypedefDeclBuilder(
        "SomeTypeAgain",
        TypeRefBuilder("double_t20").build()
        ).with({Modifier::PUBLIC, Modifier::OPEN, Modifier::STATIC}).build();
    const auto str3 = toString(typedefDecl3);
    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(str3), KahwaFileBuilder().with(typedefDecl3).build());

    expectNoDiagnostics();
}

TEST_F(TypedefTest, ParsesMultipleTypedefsCorrectly) {
    const auto typedefDecl1 = TypedefDeclBuilder("myInt", TypeRefBuilder("int").build()).build();
    const auto str1 = toString(typedefDecl1);

    const auto typedefDecl2 = TypedefDeclBuilder("SomeType", TypeRefBuilder("someOtherType").build()).with(Modifier::PRIVATE).build();
    const auto str2 = toString(typedefDecl2);

    const auto typedefDecl3 = TypedefDeclBuilder("SomeTypeAgain", TypeRefBuilder("double_t20").build()).with({Modifier::PUBLIC, Modifier::OPEN, Modifier::STATIC}).build();
    const auto str3 = toString(typedefDecl3);

    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(str1 + str2), KahwaFileBuilder().with({typedefDecl1, typedefDecl2}).build());
    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(str1 + str3), KahwaFileBuilder().with({typedefDecl1, typedefDecl3}).build());
    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(str2 + str3), KahwaFileBuilder().with({typedefDecl2, typedefDecl3}).build());
    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(str1 + str2 + str3), KahwaFileBuilder().with({typedefDecl1, typedefDecl2, typedefDecl3}).build());

    expectNoDiagnostics();
}

TEST_F(TypedefTest, ParsesGenericTypesCorrectly) {
    const auto typedefDecl1 = TypedefDeclBuilder(
        "customType",
        TypeRefBuilder("vector")
        .with(TypeRefBuilder("int").build())
        .build())
    .build();
    const auto str1 = toString(typedefDecl1);

    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(str1), KahwaFileBuilder().with(typedefDecl1).build());

    const auto typedefDecl2 = TypedefDeclBuilder(
        "multipleArgs",
        TypeRefBuilder("variant")
        .with(TypeRefBuilder("arg1").build())
        .with(TypeRefBuilder("arg2").build())
        .with(TypeRefBuilder("arg3").build())
        .build())
    .build();
    const auto str2 = toString(typedefDecl2);

    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(str2), KahwaFileBuilder().with(typedefDecl2).build());

    const auto typedefDecl3 = TypedefDeclBuilder(
        "nested",
        TypeRefBuilder("vector")
        .with(TypeRefBuilder("pair")
            .with(TypeRefBuilder("int").build())
            .with(TypeRefBuilder("bool").build())
            .build())
        .build())
    .build();
    const auto str3 = toString(typedefDecl3);

    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(str3), KahwaFileBuilder().with(typedefDecl3).build());

    const auto typedefDecl4 = TypedefDeclBuilder(
        "nestedWithMoreArgs",
        TypeRefBuilder("vector")
        .with(TypeRefBuilder("pair")
            .with(TypeRefBuilder("pair")
                .with(TypeRefBuilder("int").build())
                .with(TypeRefBuilder("bool").build())
                .build())
            .with(TypeRefBuilder("bool").build())
            .build())
        .build())
    .build();
    const auto str4 = toString(typedefDecl4);

    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(str4), KahwaFileBuilder().with(typedefDecl4).build());

    expectNoDiagnostics();
}

TEST_F(TypedefTest, ReportsCorrectDiagnosticWhenTypedefIsMalformed) {
    const auto typedefDecl = TypedefDeclBuilder("myInt", TypeRefBuilder("int").build()).build();
    const std::string str1 = "typedef int myInt;";
    const std::string str2 = "typedef int myInt";
    const std::string str3 = "typedef 0 0";
    const std::string str4 = "typedef int 0";
    const std::string str5 = "typedef";

    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(str1), KahwaFileBuilder().with(typedefDecl).build());

    expectNoDiagnostics();

    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(str2), KahwaFileBuilder().build());

    Diagnostic expectedSemicolon = {DiagnosticSeverity::ERROR, DiagnosticKind::EXPECTED_SEMI_COLON, dummy_source};
    expectDiagnosticsIgnoreSourceRange({expectedSemicolon});

    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(str3), KahwaFileBuilder().build());

    Diagnostic expectedIdentifier = {DiagnosticSeverity::ERROR, DiagnosticKind::EXPECTED_IDENTIFIER, dummy_source};
    expectDiagnosticsIgnoreSourceRange({ expectedIdentifier});

    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(str4), KahwaFileBuilder().build());
    expectDiagnosticsIgnoreSourceRange({ expectedIdentifier});

    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(str5), KahwaFileBuilder().build());
    expectDiagnosticsIgnoreSourceRange({expectedIdentifier});

    const std::string str6 = "typedef 0 int";
    Diagnostic expectedDeclaration = {DiagnosticSeverity::ERROR, DiagnosticKind::EXPECTED_DECLARATION, dummy_source};

    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(str6), KahwaFileBuilder().build());
    expectDiagnosticsIgnoreSourceRange({expectedIdentifier, expectedDeclaration});
}

TEST_F(TypedefTest, ReportsCorrectDiagnosticWhenTypedefIsMalformedWithGenerics) {
    const auto typedefDecl = TypedefDeclBuilder(
        "pid",
        TypeRefBuilder("pair")
        .with(TypeRefBuilder("int").build())
        .with(TypeRefBuilder("double").build())
        .build()).build();

    const auto str = toString(typedefDecl);
    const std::vector<std::string> strs{
        "pair<", // identifier
        "pair<int", // greater
        "pair<int, ", // identifier
        "pair<int, double", // greater
        "pair<int, double,", // identifier
        "pair<,>", // identifier
        "pair<int double>", // greater, declaration
        "pair<int, double, >", // identifier
        "pair<pair<int, double>", // greater
        "pair<>", // not sure, TODO - Do something about it
        "pair<,,>"}; // identifier

    const std::vector<std::vector<DiagnosticKind>> expectedDiagnosticKinds{
        {DiagnosticKind::EXPECTED_IDENTIFIER},
        {DiagnosticKind::EXPECTED_GREATER},
        {DiagnosticKind::EXPECTED_IDENTIFIER},
        {DiagnosticKind::EXPECTED_GREATER},
        {DiagnosticKind::EXPECTED_IDENTIFIER},
        {DiagnosticKind::EXPECTED_IDENTIFIER},
        {DiagnosticKind::EXPECTED_GREATER, DiagnosticKind::EXPECTED_DECLARATION},
        {DiagnosticKind::EXPECTED_IDENTIFIER},
        {DiagnosticKind::EXPECTED_GREATER},
        {}, // TODO - Give error maybe
        {DiagnosticKind::EXPECTED_IDENTIFIER}
    };

    EXPECT_EQ(strs.size(), expectedDiagnosticKinds.size());

    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(str), KahwaFileBuilder().with(typedefDecl).build());

    expectNoDiagnostics();

    for (int i = 0; i < strs.size(); i++) {
        EXPECT_PRED2(typeRefEqualIgnoreSourceRange, parseTypeRef(strs[i]), nullptr);

        expectDiagnosticKindsIgnoreSourceRange(expectedDiagnosticKinds[i]);
    }
}

TEST_F(TypedefTest, RecoversFromMalformedTypedefCorrectly) {
    const std::string str1 = " typedef int myInt ";
    const std::string str2 = " typedef 0 0 ";
    const std::string str3 = " typedef int 0 ";
    const auto typedefDecl = TypedefDeclBuilder("workNow", TypeRefBuilder("should").build()).build();
    const auto validStr = toString(typedefDecl);

    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(str1 + validStr), KahwaFileBuilder().with(typedefDecl).build());
    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(str2 + validStr), KahwaFileBuilder().with(typedefDecl).build());
    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(str3 + validStr), KahwaFileBuilder().with(typedefDecl).build());
    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(validStr + str1), KahwaFileBuilder().with(typedefDecl).build());
    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(validStr + str1 + str2 + str3), KahwaFileBuilder().with(typedefDecl).build());
    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(validStr + str1 + validStr + str2), KahwaFileBuilder().with({typedefDecl, typedefDecl}).build());
    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(validStr + str1 + validStr + str2 + str3 + validStr), KahwaFileBuilder().with({3, typedefDecl}).build());

    const std::string str4 = " private typedef ";
    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(str4 + validStr), KahwaFileBuilder().with(typedefDecl).build());
}

TEST_F(TypedefTest, RecoversFromMalformedTypedefCorrectlyWithGenerics) {
    // TODO
}