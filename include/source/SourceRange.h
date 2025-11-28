//
// Created by Agamjeet Singh on 28/11/25.
//

#ifndef SOURCERANGE_H
#define SOURCERANGE_H
#include <cassert>

#include "SourceLocation.h"


struct SourceRange {
    const int file_id;
    const int pos;
    const int length;

    SourceRange(int file_id, int pos, int length = 1): file_id(file_id), pos(pos), length(length) {}

    explicit SourceRange(const SourceLocation source_location): file_id(source_location.file_id), pos(source_location.pos), length(1) {}

    SourceRange(const SourceLocation start_source_location, const SourceLocation end_source_location): file_id(start_source_location.file_id), pos(start_source_location.pos), length(end_source_location.pos - start_source_location.pos) {
        assert(start_source_location.file_id == end_source_location.file_id);
        assert(length >= 0);
    }
};



#endif //SOURCERANGE_H
