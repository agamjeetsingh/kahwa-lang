//
// Created by Agamjeet Singh on 27/11/25.
//

#ifndef SOURCEFILE_H
#define SOURCEFILE_H
#include <filesystem>


struct SourceFile {
    const std::filesystem::path path;
    const std::string contents;
};



#endif //SOURCEFILE_H
