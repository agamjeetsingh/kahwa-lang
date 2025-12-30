//
// Created by Agamjeet Singh on 27/11/25.
//

#ifndef SOURCEFILE_H
#define SOURCEFILE_H



struct SourceFile {
    explicit SourceFile(const std::string& contents): contents(contents) {}

    const std::string contents;

    std::pair<std::size_t, std::size_t> getLinePos(std::size_t offset) const {
        if (!newlineCacheBuilt) {
            newlinePositions.reserve(contents.size() / 16);
            for (std::size_t i = 0; i < contents.size(); ++i) {
                if (contents[i] == '\n') {
                    newlinePositions.push_back(i);
                }
            }
            newlineCacheBuilt = true;
        }

        auto it = std::ranges::upper_bound(newlinePositions, offset);
        std::size_t line = it - newlinePositions.begin();
        std::size_t lineStart = (line == 0) ? 0 : newlinePositions[line - 1] + 1;
        std::size_t column = offset - lineStart;
        return {line, column};
    }

private:
    mutable bool newlineCacheBuilt = false;
    mutable std::vector<std::size_t> newlinePositions;
};



#endif //SOURCEFILE_H
