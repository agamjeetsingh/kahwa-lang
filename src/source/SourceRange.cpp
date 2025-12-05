//
// Created by Agamjeet Singh on 28/11/25.
//

#include "../../include/source/SourceRange.h"
#include "../../include/source/SourceLocation.h"
#include <cassert>
#include "../../include/tokeniser/Token.h"

SourceRange::SourceRange(std::size_t file_id, std::size_t pos, std::size_t length): file_id(file_id), pos(pos), length(length) {}

SourceRange::SourceRange(const SourceLocation source_location): file_id(source_location.file_id), pos(source_location.pos), length(1) {}

SourceRange::SourceRange(const SourceLocation start_source_location, const SourceLocation end_source_location): file_id(start_source_location.file_id), pos(start_source_location.pos), length(end_source_location.pos - start_source_location.pos) {
    assert(start_source_location.file_id == end_source_location.file_id);
}

SourceRange::SourceRange(const Token& first, const Token& last): file_id(first.source_range.file_id), pos(first.source_range.pos), length(last.source_range.length + last.source_range.pos - first.source_range.pos) {
    assert(first.source_range.file_id == last.source_range.file_id);
    assert(first.source_range.pos <= last.source_range.pos);
}

bool SourceRange::operator==(const SourceRange &other) const {
    return file_id == other.file_id
           && pos == other.pos
           && length == other.length;
}