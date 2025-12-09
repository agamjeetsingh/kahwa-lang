//
// Created by Agamjeet Singh on 28/11/25.
//

#include "../../include/source/SourceLocation.h"
#include "../../include/source/SourceRange.h"

SourceLocation SourceLocation::getEndOf(SourceRange source_range) {
    return SourceLocation{source_range.file_id, source_range.pos + source_range.length};
}