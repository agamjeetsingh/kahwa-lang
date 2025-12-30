//
// Created by Agamjeet Singh on 27/11/25.
//

#include "../../include/source/SourceManager.h"

std::size_t SourceManager::addFile(const std::filesystem::path &path) {
    const auto canonical_path = std::filesystem::canonical(path);

    std::ifstream in(canonical_path);
    std::string contents{std::istreambuf_iterator(in), std::istreambuf_iterator<char>()};

    const std::size_t id = source_files.size();
    source_files.emplace_back(contents);

    return id;
}

std::size_t SourceManager::addFile(const std::string &fileContents) {
    const std::size_t id = source_files.size();
    source_files.emplace_back(fileContents);

    return id;
}


const std::string &SourceManager::getSource(const std::size_t file_id) const {
    assert(file_id < source_files.size());
    return source_files[file_id].contents;
}

SourceFile &SourceManager::getSourceFile(std::size_t file_id) {
    assert(file_id < source_files.size());
    return source_files[file_id];
}


