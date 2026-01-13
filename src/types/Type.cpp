//
// Created by Agamjeet Singh on 20/12/25.
//

#include "../../include/types/Type.h"


Type *Type::substitute(const std::vector<TypeParameterSymbol *>& subTypeBluePrint, const Type *superType, const Type *actualSubType) {
    auto builder = TypeBuilder(superType->typeSymbol);

    if (auto typeParameter = dynamic_cast<TypeParameterSymbol*>(superType->typeSymbol)) {
        const auto it = std::ranges::find_if(subTypeBluePrint, [typeParameter](TypeParameterSymbol* typeParameterSymbol) {
            return *typeParameterSymbol == *typeParameter;
        });
        return it != subTypeBluePrint.end() ? actualSubType->genericArguments[std::distance(subTypeBluePrint.begin(), it)] : nullptr;
    }

    for (auto genericArg: superType->genericArguments) {
        if (auto substitutedType = substitute(subTypeBluePrint, genericArg, actualSubType)) {
            builder.with(substitutedType);
        } else {
            return nullptr;
        }
    }

    return builder.build();
}
