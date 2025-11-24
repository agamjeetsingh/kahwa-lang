//
// Created by Agamjeet Singh on 24/11/25.
//

#ifndef UNTERMINATEDSTRINGLITERALERROR_H
#define UNTERMINATEDSTRINGLITERALERROR_H
#include "LexicalError.h"


class UnterminatedStringLiteralError final : public LexicalError {
public:
    explicit UnterminatedStringLiteralError(): LexicalError("Unterminated string literal.") {}
};



#endif //UNTERMINATEDSTRINGLITERALERROR_H
