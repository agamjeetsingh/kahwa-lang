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
    void reportProblem(DiagnosticSeverity kind, SourceLocation location, const std::string& msg) {
        diagnostics.emplace_back(kind, location, msg);
    }

    const std::vector<Diagnostic>& getAll() { return diagnostics; }

private:
    std::vector<Diagnostic> diagnostics;
};



#endif //DIAGNOSTICENGINE_H
