//
// Created by Agamjeet Singh on 28/11/25.
//

#ifndef SOURCELOCATION_H
#define SOURCELOCATION_H

#include <cstddef>

struct SourceRange;

struct SourceLocation {
    const std::size_t file_id;
    const std::size_t pos;

    static SourceLocation getEndOf(SourceRange source_range);
};



#endif //SOURCELOCATION_H
