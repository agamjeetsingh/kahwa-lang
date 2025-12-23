//
// Created by Agamjeet Singh on 19/12/25.
//

#ifndef TYPEPARAMETERSYMBOL_H
#define TYPEPARAMETERSYMBOL_H
#include <string>

#include "SymbolBuilder.h"
#include "TypeSymbol.h"
#include "../types/Variance.h"


struct TypeParameterSymbol final : TypeSymbol {
    explicit TypeParameterSymbol(std::string name, const Variance variance = Variance::INVARIANT): TypeSymbol(std::move(name)), variance(variance) {}

    const Variance variance;

    bool operator==(const TypeParameterSymbol &other) const {
        return name == other.name;
    }
};

class TypeParameterSymbolBuilder : public SymbolBuilder {
public:
    explicit TypeParameterSymbolBuilder(std::string name, const Variance variance = Variance::INVARIANT): name(std::move(name)), variance(variance) {}

    TypeParameterSymbol* build() {
        return arena->make<TypeParameterSymbol>(name, variance);
    }

    TypeParameterSymbolBuilder& with(Variance variance) {
        this->variance = variance;
        return *this;
    }

private:
    std::string name;
    Variance variance = Variance::INVARIANT;
};



#endif //TYPEPARAMETERSYMBOL_H
