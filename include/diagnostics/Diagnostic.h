//
// Created by Agamjeet Singh on 27/11/25.
//

#ifndef DIAGNOSTIC_H
#define DIAGNOSTIC_H
#include <string>

#include "DiagnosticKind.h"
#include "DiagnosticSeverity.h"
#include "../source/SourceLocation.h"
#include "../source/SourceRange.h"


struct Diagnostic {
    const DiagnosticSeverity severity;
    const DiagnosticKind kind;
    const SourceRange source_range;
    const std::string msg;

    bool operator==(const Diagnostic& other) const {
        return severity == other.severity &&
               kind == other.kind &&
               source_range == other.source_range &&
                   msg == other.msg;
    }
};



#endif //DIAGNOSTIC_H
