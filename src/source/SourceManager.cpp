//
// Created by Agamjeet Singh on 27/11/25.
//

#include "../../include/source/SourceManager.h"

std::size_t SourceManager::addFile(const std::filesystem::path &path) {
    const auto canonical_path = std::filesystem::canonical(path);

    std::ifstream in(canonical_path);
    std::string contents{std::istreambuf_iterator(in), std::istreambuf_iterator<char>()};

    const std::size_t id = next_id++;
    source_files.emplace(id, contents);

    return id;
}

std::size_t SourceManager::addFile(const std::string &fileContents) {
    const std::size_t id = next_id++;
    source_files.emplace(id, fileContents);

    return id;
}

void SourceManager::removeFile(std::size_t id) {
    if (source_files.contains(id)) {
        source_files.erase(id);
    }
}



const std::string &SourceManager::getSource(const std::size_t file_id) {
    if (source_files.contains(file_id)) return source_files.at(file_id).contents;

    return "FILE NOT FOUND";
}

SourceFile &SourceManager::getSourceFile(std::size_t file_id) {
    assert(source_files.contains(file_id));

    return source_files.at(file_id);
}


