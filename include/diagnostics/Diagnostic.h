//
// Created by Agamjeet Singh on 27/11/25.
//

#ifndef DIAGNOSTIC_H
#define DIAGNOSTIC_H
#include <string>

#include "DiagnosticSeverity.h"
#include "../source/SourceLocation.h"


struct Diagnostic {
    const DiagnosticSeverity kind;
    const SourceLocation source_location;
    const std::string msg;
};



#endif //DIAGNOSTIC_H
