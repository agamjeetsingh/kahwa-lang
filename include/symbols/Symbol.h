//
// Created by Agamjeet Singh on 21/12/25.
//

#ifndef SYMBOL_H
#define SYMBOL_H
#include <string>


struct Symbol {
    explicit Symbol(std::string name): name(std::move(name)) {}

    const std::string name;

    virtual ~Symbol() noexcept = default;
};



#endif //SYMBOL_H
