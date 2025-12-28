//
// Created by Agamjeet Singh on 04/12/25.
//

#include <gtest/gtest.h>

#include "../DiagnosticTesting.h"
#include "../../include/diagnostics/DiagnosticEngine.h"
#include "../../include/parser/Parser.h"
#include "../../include/tokeniser/Tokeniser.h"
#include "../../include/parser/Modifier.h"
#include "ParserTestBase.cpp"

class FunctionTest : public testing::Test, public ParserTestBase {};

TEST_F(FunctionTest, ParsesTopLevelMethodsCorrectly) {
    const auto methodDecl1 = MethodDeclBuilder(
        "foo",
        TypeRefBuilder("float").build(),
        BlockBuilder().build()).build();
    const auto str1 = toString(methodDecl1);

    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange,
        parseFile(str1),
        KahwaFileBuilder().with(methodDecl1).build());

    const auto methodDecl2 = MethodDeclBuilder(
        "foo",
        TypeRefBuilder("int").build(),
        BlockBuilder().build())
        .with(Modifier::PRIVATE)
        .with(Modifier::STATIC)
        .with({TypeRefBuilder("int").build(), "length"})
        .with({TypeRefBuilder("vector") // vector<int>
            .with(TypeRefBuilder("int").build())
            .build(), "list"})
        .build();
    const auto str2 = toString(methodDecl2);

    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange,
        parseFile(str2),
        KahwaFileBuilder().with(methodDecl2).build());

    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange,
        parseFile(str1 + str2),
        KahwaFileBuilder().with({methodDecl1, methodDecl2}).build());

    expectNoDiagnostics();
}
