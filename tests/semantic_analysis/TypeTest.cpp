//
// Created by Agamjeet Singh on 21/12/25.
//

#include <gtest/gtest.h>

#include "../../include/types/Type.h"

class TypeTest : public testing::Test {
protected:
    ClassSymbol* intTypeSymbol = ClassSymbolBuilder("int").build();
    ClassSymbol* floatTypeSymbol = ClassSymbolBuilder("float").build();
    Type intType = Type(intTypeSymbol);
    Type floatType = Type(floatTypeSymbol);
};

TEST_F(TypeTest, CorrectlyChecksSubtypingForDirectInheritance) {
    auto typeSymbolB = ClassSymbolBuilder("B").build(); // B
    auto typeB = Type(typeSymbolB); // B

    // A <: B
    auto typeSymbolA = ClassSymbolBuilder("A").withSuperClass(&typeB).build(); // A
    auto typeA = Type(typeSymbolA); // A

    EXPECT_TRUE(typeA.isSubtypeOf(&typeB));
    EXPECT_FALSE(typeB.isSubtypeOf(&typeA));
}

TEST_F(TypeTest, SameTypeIsEqualToItself) {
    // int, A<T>, A<T, U>, A<T, U>
    std::vector typeSymbols = {
        intTypeSymbol,
        ClassSymbolBuilder("A").with("T").build(),
        ClassSymbolBuilder("A").with("T").with("U").build(),
        ClassSymbolBuilder("A").with("T").with("U").build()
    };

    // int, A<int>, A<float, int>, A<int, U>
    auto type = Type(TypeParameterSymbolBuilder("T").build());
    std::vector types = {
        Type(typeSymbols[0]),
        Type(typeSymbols[1], {&intType}),
        Type(typeSymbols[2], {&floatType, &intType}),
        Type(typeSymbols[3], {&intType, &type})
    };

    for (const auto& type: types) {
        EXPECT_TRUE(type == type);
        EXPECT_TRUE(type.isSubtypeOf(&type));
    }
}

TEST_F(TypeTest, CorrectlyChecksSubtypingForChainOfInheritance) {
    // A <: B <: C
    auto typeSymbolC = ClassSymbolBuilder("C").build();
    auto typeSymbolB = ClassSymbolBuilder("B").withSuperClass(TypeBuilder(typeSymbolC).build()).build();
    auto typeSymbolA = ClassSymbolBuilder("A").withSuperClass(TypeBuilder(typeSymbolB).build()).build();

    auto typeC = TypeBuilder(typeSymbolC).build();
    auto typeB = TypeBuilder(typeSymbolB).build();
    auto typeA = TypeBuilder(typeSymbolA).build();

    EXPECT_TRUE(typeA->isSubtypeOf(typeB));
    EXPECT_TRUE(typeB->isSubtypeOf(typeC));
    EXPECT_TRUE(typeA->isSubtypeOf(typeC));
    EXPECT_FALSE(typeB->isSubtypeOf(typeA));
    EXPECT_FALSE(typeC->isSubtypeOf(typeB));
    EXPECT_FALSE(typeC->isSubtypeOf(typeA));
}

TEST_F(TypeTest, CorrectlyChecksSubtypingForDirectInheritanceWithGenerics) {
    // B<T>
    auto typeSymbolB = ClassSymbolBuilder("B").with("T").build();
    // B<int>
    auto typeB1 = TypeBuilder(typeSymbolB).with(&intType).build();
    // B<U>
    auto typeB2 = TypeBuilder(typeSymbolB).with(TypeBuilder("U").build()).build();

    // A<U>
    auto typeSymbolA = ClassSymbolBuilder("A").with("U").withSuperClass(typeB2).build();
    // A<int>
    auto typeA1 = TypeBuilder(typeSymbolA).with(&intType).build();
    // A<U>
    auto typeA2 = TypeBuilder(typeSymbolA).with(TypeBuilder("U").build()).build();

    EXPECT_TRUE(typeA1->isSubtypeOf(typeB1)); // B<int> <: A<int>
    EXPECT_TRUE(typeA2->isSubtypeOf(typeB2)); // A<U> <: B<U>
}
