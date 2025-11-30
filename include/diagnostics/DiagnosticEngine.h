//
// Created by Agamjeet Singh on 27/11/25.
//

#ifndef DIAGNOSTICENGINE_H
#define DIAGNOSTICENGINE_H
#include <string>
#include <vector>

#include "Diagnostic.h"
#include "../source/SourceLocation.h"


class DiagnosticEngine {
public:
    void reportProblem(DiagnosticSeverity severity, DiagnosticKind kind, const SourceLocation location, const std::string& msg) {
        diagnostics.emplace_back(severity, kind, SourceRange{location}, msg);
    }

    void reportProblem(DiagnosticSeverity severity, DiagnosticKind kind, SourceRange range, const std::string& msg) {
        diagnostics.emplace_back(severity, kind, range, msg);
    }

    [[nodiscard]] const std::vector<Diagnostic>& getAll() const { return diagnostics; }

private:
    std::vector<Diagnostic> diagnostics;
};



#endif //DIAGNOSTICENGINE_H
