//
// Created by Agamjeet Singh on 24/11/25.
//

#include "../../../include/parser/exceptions/CompilationError.h"

const char *CompilationError::what() const noexcept {
    return msg.c_str();
}
