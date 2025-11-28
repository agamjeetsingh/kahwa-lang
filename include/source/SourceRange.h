//
// Created by Agamjeet Singh on 28/11/25.
//

#ifndef SOURCERANGE_H
#define SOURCERANGE_H
#include <cassert>

#include "SourceLocation.h"


struct SourceRange {
    const std::size_t file_id;
    const std::size_t pos;
    const int length;

    SourceRange(std::size_t file_id, std::size_t pos, int length = 1): file_id(file_id), pos(pos), length(length) {}

    explicit SourceRange(const SourceLocation source_location): file_id(source_location.file_id), pos(source_location.pos), length(1) {}

    SourceRange(const SourceLocation start_source_location, const SourceLocation end_source_location): file_id(start_source_location.file_id), pos(start_source_location.pos), length(end_source_location.pos - start_source_location.pos) {
        assert(start_source_location.file_id == end_source_location.file_id);
        assert(length >= 0);
    }
};



#endif //SOURCERANGE_H
