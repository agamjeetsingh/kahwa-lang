//
// Created by Agamjeet Singh on 01/12/25.
//

#include <gtest/gtest.h>

#include "../../include/diagnostics/DiagnosticEngine.h"

class DiagnosticEngineTest : public testing::Test {
protected:
    DiagnosticEngine diagnostic_engine;

    std::vector<Diagnostic> diagnostics;
};

TEST_F(DiagnosticEngineTest, CanReportAndRetrieveDiagnosticsWithSourceLocation) {
    diagnostic_engine.reportProblem(DiagnosticSeverity::ERROR, DiagnosticKind::UNTERMINATED_STRING_LITERAL, SourceLocation{5, 10}, "My message");
    diagnostics.emplace_back(DiagnosticSeverity::ERROR, DiagnosticKind::UNTERMINATED_STRING_LITERAL, SourceRange{SourceLocation{5, 10}}, "My message");

    ASSERT_EQ (diagnostic_engine.getAll(), diagnostics);

    diagnostic_engine.reportProblem(DiagnosticSeverity::WARNING, DiagnosticKind::UNRECOGNISED_TOKEN, SourceLocation{10, 20}, "Another message");
    diagnostics.emplace_back(DiagnosticSeverity::WARNING, DiagnosticKind::UNRECOGNISED_TOKEN, SourceRange{SourceLocation{10, 20}}, "Another message");

    ASSERT_EQ (diagnostic_engine.getAll(), diagnostics);
}

TEST_F(DiagnosticEngineTest, CanReportAndRetrieveDiagnosticsWithSourceRange) {
    diagnostic_engine.reportProblem(DiagnosticSeverity::ERROR, DiagnosticKind::UNTERMINATED_STRING_LITERAL, SourceRange{5, 10, 20}, "My message");
    diagnostics.emplace_back(DiagnosticSeverity::ERROR, DiagnosticKind::UNTERMINATED_STRING_LITERAL, SourceRange{5, 10, 20}, "My message");

    ASSERT_EQ (diagnostic_engine.getAll(), diagnostics);

    diagnostic_engine.reportProblem(DiagnosticSeverity::WARNING, DiagnosticKind::UNRECOGNISED_TOKEN, SourceRange{10, 20, 120}, "Another message");
    diagnostics.emplace_back(DiagnosticSeverity::WARNING, DiagnosticKind::UNRECOGNISED_TOKEN, SourceRange{10, 20, 120}, "Another message");

    ASSERT_EQ (diagnostic_engine.getAll(), diagnostics);
}