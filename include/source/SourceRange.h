//
// Created by Agamjeet Singh on 28/11/25.
//

#ifndef SOURCERANGE_H
#define SOURCERANGE_H
#include <cstddef>

struct SourceLocation;

struct SourceRange {
    const std::size_t file_id;
    const std::size_t pos;
    const std::size_t length;

    SourceRange(std::size_t file_id, std::size_t pos, std::size_t length = 1);

    explicit SourceRange(SourceLocation source_location);

    SourceRange(SourceLocation start_source_location, const SourceLocation end_source_location);

    bool operator==(const SourceRange &other) const;
};



#endif //SOURCERANGE_H
