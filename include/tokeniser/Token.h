//
// Created by Agamjeet Singh on 22/11/25.
//

#ifndef TOKEN_H
#define TOKEN_H
#include <string>
#include <typeindex>

#include "TokenType.h"
#include "../source/SourceRange.h"
#include <magic_enum.hpp>

std::string tokenTypeToString(TokenType type);
class Token;

std::string toString(const Token& token);

class Token {
public:
    Token(const TokenType type, const SourceRange &source_range): type(type), source_range(source_range), type_index(typeid(nullptr)) {
        if (std::unordered_set{TokenType::IDENTIFIER, TokenType::STRING_LITERAL, TokenType::CHAR_LITERAL, TokenType::INTEGER, TokenType::FLOAT}.contains(type)) {
            throw std::invalid_argument("Token of type " + std::string(magic_enum::enum_name<TokenType>(type)) + " must have data associated with it.");
        }
    }

    Token(const TokenType type, std::string data, const SourceRange &source_range) : type(type), source_range(source_range), type_index(typeid(data)), data(std::make_shared<AuxData<std::string>>(std::move(data))) {
        if (type != TokenType::IDENTIFIER && type != TokenType::STRING_LITERAL && type != TokenType::BAD) {
            throw std::invalid_argument("Token of type " + std::string(magic_enum::enum_name<TokenType>(type)) + " cannot have string data.");
        }
    }

    template <typename T>
    requires (std::is_same_v<T, int> || std::is_same_v<T, float>)
    Token(const TokenType type, T data, const SourceRange &source_range) : type(type), source_range(source_range), type_index(typeid(data)), data(std::make_shared<AuxData<T>>(data)) {
        if (!(std::is_same_v<T, int> ? type == TokenType::INTEGER : type == TokenType::FLOAT) && type != TokenType::BAD) {
            throw std::invalid_argument("Token of type " + std::string(magic_enum::enum_name<TokenType>(type)) + " cannot store integers or floats.");
        }
    }

    const TokenType type;

    template <typename T>
    [[nodiscard]] const T* getIf() const {
        if (type_index != typeid(T)) return nullptr;
        auto derived = std::dynamic_pointer_cast<const AuxData<T>>(data);
        return derived ? &derived->data : nullptr;
    }

    const SourceRange source_range;

private:
    struct AuxDataBase {
        virtual ~AuxDataBase() = default;
    };

    template<typename T>
    struct AuxData final : AuxDataBase {
        explicit AuxData(T data) : data(std::move(data)) {}

        T data;
    };

    const std::type_index type_index;
    const std::shared_ptr<const AuxDataBase> data;
};



#endif //TOKEN_H
