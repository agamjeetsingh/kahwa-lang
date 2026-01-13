//
// Created by Agamjeet Singh on 28/11/25.
//

#ifndef SOURCERANGE_H
#define SOURCERANGE_H
#include <cstddef>

class Token;
struct SourceLocation;

struct SourceRange {
    std::size_t file_id;
    std::size_t pos;
    std::size_t length;

    SourceRange(std::size_t file_id, std::size_t pos, std::size_t length = 1);

    explicit SourceRange(SourceLocation source_location);

    SourceRange(SourceLocation start_source_location, SourceLocation end_source_location);

    SourceRange(const Token& first, const Token& last);

    bool operator==(const SourceRange &other) const;

    [[nodiscard]] SourceRange after() const {
        return {file_id, pos + length - 1};
    }
};



#endif //SOURCERANGE_H
