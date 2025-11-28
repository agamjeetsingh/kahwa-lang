//
// Created by Agamjeet Singh on 27/11/25.
//

#ifndef SOURCEMANAGER_H
#define SOURCEMANAGER_H

#include <cassert>
#include <string>
#include <filesystem>
#include <fstream>

#include "SourceFile.h"


class SourceManager {
public:
    std::size_t addFile(const std::filesystem::path& path);

    [[nodiscard]] const std::string& getSource(std::size_t file_id) const;

private:
    std::vector<SourceFile> source_files;
};



#endif //SOURCEMANAGER_H
