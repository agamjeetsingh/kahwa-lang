//
// Created by Agamjeet Singh on 04/12/25.
//

#include <gtest/gtest.h>

#include "../../include/diagnostics/DiagnosticEngine.h"
#include "../../include/parser/Parser.h"
#include "../../include/tokeniser/Tokeniser.h"

class ParserTest : public testing::Test {
protected:
    Arena astArena;
    DiagnosticEngine diagnostic_engine;
    Parser parser{astArena, diagnostic_engine};
    Tokeniser tokeniser{diagnostic_engine};

    void expectDiagnostics(const std::vector<Diagnostic>& diagnostics) const {
        EXPECT_EQ(diagnostic_engine.getAll(), diagnostics);
    }

    void expectNoDiagnostics() const {
        expectDiagnostics({});
    }

    KahwaFile* parseFile(const std::string &str) const {
        return parser.parseFile(tokeniser.tokenise(0, str));
    }

};

TEST_F(ParserTest, ParsesTypedefCorrectly) {
    auto file = parseFile("typedef int myInt;");
    EXPECT_EQ(*file, (*astArena.make<KahwaFile>(std::vector{astArena.make<TypedefDecl>("myInt", astArena.make<TypeRef>("int"))})));

    expectNoDiagnostics();
}