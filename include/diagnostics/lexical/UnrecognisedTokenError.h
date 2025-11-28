//
// Created by Agamjeet Singh on 24/11/25.
//

#ifndef UNRECOGNISEDTOKENERROR_H
#define UNRECOGNISEDTOKENERROR_H
#include "LexicalError.h"


class UnrecognisedTokenError : public LexicalError {
public:
    explicit UnrecognisedTokenError() : LexicalError("Unrecognised token.") {}
};



#endif //UNRECOGNISEDTOKENERROR_H
