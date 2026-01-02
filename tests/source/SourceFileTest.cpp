//
// Created by Agamjeet Singh on 02/01/26.
//

#include <gtest/gtest.h>

#include "../../include/source/SourceManager.h"

class SourceFileTest : public testing::Test {
protected:
    SourceManager sourceManager;

    std::size_t createFile(const std::string& str) {
        return sourceManager.addFile(str);
    }

    void expectLinePos(const std::vector<std::pair<std::size_t, std::pair<std::size_t, std::size_t>>>& linePosList, std::size_t id) {
        std::ranges::for_each(linePosList, [id, this](auto& pair) {
            EXPECT_EQ(pair.second, sourceManager.getSourceFile(id).getLinePos(pair.first));
        });
    }
};

TEST_F(SourceFileTest, GetsLinePosCorrectly) {
    auto id = createFile("a b cd\nef\n\n01g");
    std::vector<std::pair<std::size_t, std::pair<std::size_t, std::size_t>>> expectedLinePos = {
        {0, {0, 0}}, // 'a'
        {1, {0, 1}}, // ' '
        {2, {0, 2}}, // 'b'
        {3, {0, 3}}, // ' '
        {4, {0, 4}}, // 'c'
        {5, {0, 5}}, // 'd'
        {6, {0, 6}}, // '\n'
        {7, {1, 0}}, // 'e'
        {8, {1, 1}}, // 'f'
        {9, {1, 2}}, // '\n'
        {10, {2, 0}}, // '\n'
        {11, {3, 0}}, // '0'
        {12, {3, 1}}, // '1'
        {13, {3, 2}}, // 'g'
    };

    expectLinePos(expectedLinePos, id);
}

TEST_F(SourceFileTest, ClampsLinePosToLastCharWhenOffsetIsTooLarge) {
    std::vector<std::pair<std::size_t, std::vector<std::pair<std::size_t, std::pair<std::size_t, std::size_t>>>>> tests = {
        {createFile(""), {{0, {0, 0}}, {1, {0, 0}}}},
        {createFile("a"), {{1, {0, 0}}, {2, {0, 0}}}},
        {createFile("ab\n"), {{2, {0, 2}}, {3, {0, 2}}, {4, {0, 2}}}},
        {createFile("ab\ncd\nef"), {{7, {2, 1}}, {8, {2, 1}}, {9, {2, 1}}}},
        {createFile("a\n\n"), {{2, {1, 0}}, {3, {1, 0}}, {4, {1, 0}}}}
    };

    std::ranges::for_each(tests, [this](const auto& pair) {
       expectLinePos(pair.second, pair.first);
    });
}