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
        if (frozen) return;

        diagnostics.emplace_back(severity, kind, SourceRange{location}, msg);
        diagnosticsMap[location.file_id].emplace_back(severity, kind, SourceRange{location}, msg);

        if (severity == DiagnosticSeverity::ERROR) {
            errors.emplace_back(severity, kind, SourceRange{location}, msg);
            errorsMap[location.file_id].emplace_back(severity, kind, SourceRange{location}, msg);
        } else {
            warnings.emplace_back(severity, kind, SourceRange{location}, msg);
            warningsMap[location.file_id].emplace_back(severity, kind, SourceRange{location}, msg);
        }
    }

    void reportProblem(DiagnosticSeverity severity, DiagnosticKind kind, const SourceLocation location) {
        if (frozen) return;

        diagnostics.emplace_back(severity, kind, SourceRange{location});
        diagnosticsMap[location.file_id].emplace_back(severity, kind, SourceRange{location});

        if (severity == DiagnosticSeverity::ERROR) {
            errors.emplace_back(severity, kind, SourceRange{location});
            errorsMap[location.file_id].emplace_back(severity, kind, SourceRange{location});
        } else {
            warnings.emplace_back(severity, kind, SourceRange{location});
            warningsMap[location.file_id].emplace_back(severity, kind, SourceRange{location});
        }
    }

    void reportProblem(DiagnosticSeverity severity, DiagnosticKind kind, SourceRange range, const std::string& msg) {
        if (frozen) return;

        diagnostics.emplace_back(severity, kind, range, msg);
        diagnosticsMap[range.file_id].emplace_back(severity, kind, range, msg);

        if (severity == DiagnosticSeverity::ERROR) {
            errors.emplace_back(severity, kind, range, msg);
            errorsMap[range.file_id].emplace_back(severity, kind, range, msg);
        } else {
            warnings.emplace_back(severity, kind, range, msg);
            warningsMap[range.file_id].emplace_back(severity, kind, range, msg);
        }
    }

    void reportProblem(DiagnosticSeverity severity, DiagnosticKind kind, SourceRange range) {
        if (frozen) return;

        diagnostics.emplace_back(severity, kind, range);
        diagnosticsMap[range.file_id].emplace_back(severity, kind, range);

        if (severity == DiagnosticSeverity::ERROR) {
            errors.emplace_back(severity, kind, range);
            errorsMap[range.file_id].emplace_back(severity, kind, range);
        } else {
            warnings.emplace_back(severity, kind, range);
            warningsMap[range.file_id].emplace_back(severity, kind, range);
        }
    }

    void reportProblem(const Diagnostic& diagnostic) {
        if (frozen) return;

        diagnostics.push_back(diagnostic);
        diagnosticsMap[diagnostic.source_range.file_id].emplace_back(diagnostic);

        if (diagnostic.severity == DiagnosticSeverity::ERROR) {
            errors.push_back(diagnostic);
            errorsMap[diagnostic.source_range.file_id].emplace_back(diagnostic);
        } else {
            warnings.push_back(diagnostic);
            warningsMap[diagnostic.source_range.file_id].emplace_back(diagnostic);
        }
    }

    [[nodiscard]] const std::vector<Diagnostic>& getAll() const { return diagnostics; }

    [[nodiscard]] const std::vector<Diagnostic>& getErrors() const { return errors; }

    [[nodiscard]] const std::vector<Diagnostic>& getWarnings() const { return warnings; }

    [[nodiscard]] const std::vector<Diagnostic>& getAll(const std::size_t file_id) { return diagnosticsMap[file_id]; }

    [[nodiscard]] const std::vector<Diagnostic>& getErrors(const std::size_t file_id) { return errorsMap[file_id]; }

    [[nodiscard]] const std::vector<Diagnostic>& getWarnings(const std::size_t file_id) { return warningsMap[file_id]; }

    void clear() {
        diagnostics.clear();
        errors.clear();
        warnings.clear();
        diagnosticsMap.clear();
        errorsMap.clear();
        warningsMap.clear();
    }

    void clear(std::size_t file_id) {
        diagnosticsMap[file_id].clear();
        errorsMap[file_id].clear();
        warningsMap[file_id].clear();
    }

    void freeze() {
        frozen = true;
    }

    void unfreeze() {
        frozen = false;
    }

private:
    std::vector<Diagnostic> diagnostics;
    std::vector<Diagnostic> errors;
    std::vector<Diagnostic> warnings;
    bool frozen = false;

    std::unordered_map<std::size_t, std::vector<Diagnostic>> diagnosticsMap;
    std::unordered_map<std::size_t, std::vector<Diagnostic>> errorsMap;
    std::unordered_map<std::size_t, std::vector<Diagnostic>> warningsMap;
};



#endif //DIAGNOSTICENGINE_H
