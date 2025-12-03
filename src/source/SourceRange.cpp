//
// Created by Agamjeet Singh on 28/11/25.
//

#include "../../include/source/SourceRange.h"
#include "../../include/source/SourceLocation.h"
#include <cassert>

SourceRange::SourceRange(std::size_t file_id, std::size_t pos, std::size_t length): file_id(file_id), pos(pos), length(length) {}

SourceRange::SourceRange(const SourceLocation source_location): file_id(source_location.file_id), pos(source_location.pos), length(1) {}

SourceRange::SourceRange(const SourceLocation start_source_location, const SourceLocation end_source_location): file_id(start_source_location.file_id), pos(start_source_location.pos), length(end_source_location.pos - start_source_location.pos) {
    assert(start_source_location.file_id == end_source_location.file_id);
}

bool SourceRange::operator==(const SourceRange &other) const {
    return file_id == other.file_id
           && pos == other.pos
           && length == other.length;
}