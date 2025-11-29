#include <gtest/gtest.h>
#include "../include/tokeniser/Token.h"
#include "../include/source/SourceRange.h"

class TokenTest : public testing::Test {
protected:
    SourceRange source_range = {0, 10, 5};
    static SourceRange getTestSourceRange() {
        return {0, 10, 5};
    }
};

TEST_F(TokenTest, CreateTokenWithoutData_ShouldHaveCorrectTypeAndSourceRange) {
    Token token(TokenType::COLON, source_range);
    
    EXPECT_EQ(token.type, TokenType::COLON);
    EXPECT_EQ(token.source_range.file_id, 0);
    EXPECT_EQ(token.source_range.pos, 10);
    EXPECT_EQ(token.source_range.length, 5);
}

TEST_F(TokenTest, CreateTokenWithStringData_ShouldStoreAndRetrieveCorrectly) {
    const std::string identifier_name = "myVariable";
    Token token(TokenType::IDENTIFIER, identifier_name, source_range);
    
    EXPECT_EQ(token.type, TokenType::IDENTIFIER);
    
    const auto* retrieved = token.getIf<std::string>();
    ASSERT_NE(retrieved, nullptr);
    EXPECT_EQ(*retrieved, identifier_name);
}

TEST_F(TokenTest, CreateTokenWithIntData_ShouldStoreAndRetrieveCorrectly) {
    const int integer_value = 42;
    Token token(TokenType::INTEGER, integer_value, source_range);
    
    EXPECT_EQ(token.type, TokenType::INTEGER);
    
    const int* retrieved = token.getIf<int>();
    ASSERT_NE(retrieved, nullptr);
    EXPECT_EQ(*retrieved, integer_value);
}

TEST_F(TokenTest, CreateTokenWithFloatData_ShouldStoreAndRetrieveCorrectly) {
    const float float_value = 3.14f;
    Token token(TokenType::FLOAT, float_value, source_range);
    
    EXPECT_EQ(token.type, TokenType::FLOAT);
    
    const auto* retrieved = token.getIf<float>();
    ASSERT_NE(retrieved, nullptr);
    EXPECT_FLOAT_EQ(*retrieved, float_value);
}

TEST_F(TokenTest, GetIfWithWrongType_ShouldReturnNull) {
    const std::string identifier_name = "myVariable";
    Token token(TokenType::IDENTIFIER, identifier_name, source_range);
    
    const int* retrieved_as_int = token.getIf<int>();
    EXPECT_EQ(retrieved_as_int, nullptr);
    
    const float* retrieved_as_float = token.getIf<float>();
    EXPECT_EQ(retrieved_as_float, nullptr);
}

TEST_F(TokenTest, TokenWithoutData_GetIfShouldReturnNull) {
    Token token(TokenType::COLON, source_range);
    
    const std::string* retrieved_string = token.getIf<std::string>();
    EXPECT_EQ(retrieved_string, nullptr);
    
    const int* retrieved_int = token.getIf<int>();
    EXPECT_EQ(retrieved_int, nullptr);
    
    const float* retrieved_float = token.getIf<float>();
    EXPECT_EQ(retrieved_float, nullptr);
}