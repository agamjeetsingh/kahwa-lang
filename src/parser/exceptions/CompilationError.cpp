//
// Created by Agamjeet Singh on 24/11/25.
//

#include "../../../include/diagnostics/CompilationError.h"

const char *CompilationError::what() const noexcept {
    return msg.c_str();
}
