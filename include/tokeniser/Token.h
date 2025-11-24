//
// Created by Agamjeet Singh on 22/11/25.
//

#ifndef TOKEN_H
#define TOKEN_H
#include <string>
#include <typeindex>

#include "TokenType.h"

std::string tokenTypeToString(TokenType type);
class Token;

std::string tokenToString(const Token& token);

class Token {
public:
    explicit Token(const TokenType type, const std::size_t start, const std::size_t length = 1): type(type), start(start) , end(start + length), type_index(typeid(nullptr)) {}

    Token(const TokenType type, std::string data, const std::size_t start, const std::size_t length = 1) : type(type), start(start) , end(start + length), type_index(typeid(data)), data(std::make_shared<AuxData<std::string>>(std::move(data))) {}

    template <typename T>
    requires (std::is_same_v<T, int> || std::is_same_v<T, float>)
    Token(const TokenType type, T data, const std::size_t start, const std::size_t length = 1) : type(type), start(start) , end(start + length), type_index(typeid(data)), data(std::make_shared<AuxData<T>>(data)) {}

    const TokenType type;

    template <typename T>
    [[nodiscard]] const T* getIf() const {
        if (type_index != typeid(T)) return nullptr;
        auto derived = std::dynamic_pointer_cast<const AuxData<T>>(data);
        return derived ? &derived->data : nullptr;
    }

    const std::size_t start;
    const std::size_t end; // exclusive

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
