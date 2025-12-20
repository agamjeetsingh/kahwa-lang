//
// Created by Agamjeet Singh on 19/12/25.
//

#ifndef DIAGNOSTICTESTING_H
#define DIAGNOSTICTESTING_H

#include <gtest/gtest.h>

#include "../include/diagnostics/DiagnosticEngine.h"

enum class DiagnosticKind;
struct Diagnostic;

class DiagnosticTesting {
protected:
    DiagnosticEngine diagnostic_engine;

    void expectErrorsIgnoreSourceRange(const std::vector<std::pair<DiagnosticKind, std::string>>& diagnostics) {
        auto& actualDiagnostics = diagnostic_engine.getAll();
        std::vector<Diagnostic> expectedDiagnostics;
        expectedDiagnostics.reserve(diagnostics.size());
        for (auto pair: diagnostics) {
            expectedDiagnostics.emplace_back(DiagnosticSeverity::ERROR, pair.first, dummy_source, pair.second);
        }
        EXPECT_EQ(actualDiagnostics.size(), alreadyExpected + expectedDiagnostics.size());
        EXPECT_TRUE((std::equal(actualDiagnostics.end() - expectedDiagnostics.size(), actualDiagnostics.end(), expectedDiagnostics.begin())));
        alreadyExpected += expectedDiagnostics.size();
    }

    void expectDiagnostics(const std::vector<Diagnostic>& diagnostics) {
        auto& actualDiagnostics = diagnostic_engine.getAll();
        EXPECT_EQ(actualDiagnostics.size(), alreadyExpected + diagnostics.size());
        EXPECT_TRUE((std::equal(actualDiagnostics.end() - diagnostics.size(), actualDiagnostics.end(), diagnostics.begin())));
        alreadyExpected += diagnostics.size();
    }

    void expectDiagnosticsIgnoreSourceRange(const std::vector<Diagnostic>& diagnostics) {
        auto& actualDiagnostics = diagnostic_engine.getAll();
        EXPECT_EQ(actualDiagnostics.size(), alreadyExpected + diagnostics.size());

        for (std::size_t i = alreadyExpected; i < actualDiagnostics.size(); i++) {
            EXPECT_EQ(actualDiagnostics[i].severity, diagnostics[i - alreadyExpected].severity);
            EXPECT_EQ(actualDiagnostics[i].kind, diagnostics[i - alreadyExpected].kind);
            EXPECT_EQ(actualDiagnostics[i].msg, diagnostics[i - alreadyExpected].msg);
        }
        alreadyExpected += diagnostics.size();
    }

    void expectDiagnosticKindsIgnoreSourceRange(const std::vector<DiagnosticKind>& diagnosticKinds) {
        std::vector<Diagnostic> diagnostics;
        for (auto kind: diagnosticKinds) {
            diagnostics.emplace_back(DiagnosticSeverity::ERROR, kind, dummy_source);
        }

        expectDiagnosticsIgnoreSourceRange(diagnostics);
    }

    std::size_t alreadyExpected = 0;

    void expectNoDiagnostics() {
        expectDiagnostics({});
    }

    static inline SourceRange dummy_source = {0, 0};
};



#endif //DIAGNOSTICTESTING_H
