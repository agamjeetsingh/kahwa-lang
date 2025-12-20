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

        if (severity == DiagnosticSeverity::ERROR) {
            errors.emplace_back(severity, kind, SourceRange{location}, msg);
        } else {
            warnings.emplace_back(severity, kind, SourceRange{location}, msg);
        }
    }

    void reportProblem(DiagnosticSeverity severity, DiagnosticKind kind, const SourceLocation location) {
        diagnostics.emplace_back(severity, kind, SourceRange{location});

        if (severity == DiagnosticSeverity::ERROR) {
            errors.emplace_back(severity, kind, SourceRange{location});
        } else {
            warnings.emplace_back(severity, kind, SourceRange{location});
        }
    }

    void reportProblem(DiagnosticSeverity severity, DiagnosticKind kind, SourceRange range, const std::string& msg) {
        diagnostics.emplace_back(severity, kind, range, msg);

        if (severity == DiagnosticSeverity::ERROR) {
            errors.emplace_back(severity, kind, range, msg);
        } else {
            warnings.emplace_back(severity, kind, range, msg);
        }
    }

    void reportProblem(DiagnosticSeverity severity, DiagnosticKind kind, SourceRange range) {
        diagnostics.emplace_back(severity, kind, range);

        if (severity == DiagnosticSeverity::ERROR) {
            errors.emplace_back(severity, kind, range);
        } else {
            warnings.emplace_back(severity, kind, range);
        }
    }

    void reportProblem(const Diagnostic& diagnostic) {
        diagnostics.push_back(diagnostic);

        if (diagnostic.severity == DiagnosticSeverity::ERROR) {
            errors.push_back(diagnostic);
        } else {
            warnings.push_back(diagnostic);
        }
    }

    [[nodiscard]] const std::vector<Diagnostic>& getAll() const { return diagnostics; }

    [[nodiscard]] const std::vector<Diagnostic>& getErrors() const { return errors; }

    [[nodiscard]] const std::vector<Diagnostic>& getWarnings() const { return warnings; }

private:
    std::vector<Diagnostic> diagnostics;
    std::vector<Diagnostic> errors;
    std::vector<Diagnostic> warnings;
};



#endif //DIAGNOSTICENGINE_H
