//
// Created by Agamjeet Singh on 24/11/25.
//

#ifndef COMPILATIONERROR_H
#define COMPILATIONERROR_H
#include <exception>
#include <string>


class CompilationError : public std::exception {
public:
    explicit CompilationError(std::string msg): msg(std::move(msg)) {}
private:
    std::string msg;

    [[nodiscard]] const char *what() const noexcept override;
};



#endif //COMPILATIONERROR_H
