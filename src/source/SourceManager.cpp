//
// Created by Agamjeet Singh on 27/11/25.
//

#include "../../include/source/SourceManager.h"

std::size_t SourceManager::addFile(const std::filesystem::path &path) {
    auto canonical_path = std::filesystem::canonical(path);
    if (const auto it = std::ranges::find_if(source_files, [canonical_path](const SourceFile& file) { return file.path == canonical_path; }); it != source_files.end()) {
        return std::distance(source_files.begin(), it);
    }

    std::ifstream in(canonical_path);
    std::string contents{std::istreambuf_iterator(in), std::istreambuf_iterator<char>()};

    std::size_t id = source_files.size();
    source_files.emplace_back(canonical_path, contents);

    return id;
}

const std::string &SourceManager::getSource(const std::size_t file_id) const {
    assert(file_id < source_files.size());
    return source_files[file_id].contents;
}

