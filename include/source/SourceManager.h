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

    std::size_t addFile(const std::string& fileContents);

    void removeFile(std::size_t id);

    [[nodiscard]] const std::string& getSource(std::size_t file_id);

    [[nodiscard]] SourceFile& getSourceFile(std::size_t file_id);

private:
    std::unordered_map<std::size_t, SourceFile> source_files;

    std::size_t next_id = 0;
};



#endif //SOURCEMANAGER_H
