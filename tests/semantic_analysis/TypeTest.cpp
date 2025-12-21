//
// Created by Agamjeet Singh on 21/12/25.
//

#include <gtest/gtest.h>

#include "../../include/types/Type.h"

class TypeTest : public testing::Test {
protected:
    ClassSymbol* numberTypeSymbol = ClassSymbolBuilder("number").build();
    Type* numberType = TypeBuilder(numberTypeSymbol).build();
    ClassSymbol* intTypeSymbol = ClassSymbolBuilder("int").withSuperClass(numberType).build();
    ClassSymbol* floatTypeSymbol = ClassSymbolBuilder("float").withSuperClass(numberType).build();
    Type* intType = TypeBuilder(intTypeSymbol).build();
    Type* floatType = TypeBuilder(floatTypeSymbol).build();

    static void testSubtypeRelations(const std::vector<Type*>& types, const std::vector<std::pair<Type*, Type*>>& expectedSubtypes) {
        for (Type* subtype : types) {
            for (Type* supertype : types) {
                if (subtype == supertype) {
                    EXPECT_TRUE(subtype->isSubtypeOf(supertype));
                    continue;
                }
                bool expected = std::ranges::find(expectedSubtypes, std::make_pair(subtype, supertype)) != expectedSubtypes.end();
                EXPECT_EQ(subtype->isSubtypeOf(supertype), expected);
                assert(subtype->isSubtypeOf(supertype) == expected);
            }
        }
    }
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
    std::vector types = {
        TypeBuilder(typeSymbols[0]).build(),
        TypeBuilder(typeSymbols[1]).with(intType).build(),
        TypeBuilder(typeSymbols[2]).with({floatType, intType}).build(),
        TypeBuilder(typeSymbols[3]).with({intType, TypeBuilder("U").build()}).build()
    };

    for (const auto& type: types) {
        EXPECT_TRUE(type == type);
        EXPECT_TRUE(type->isSubtypeOf(type));
    }
}

TEST_F(TypeTest, CorrectlyChecksSubtypingForGenericVarianceWithOneParameter) {
    // A<T>
    auto typeSymbolA = ClassSymbolBuilder("A").with("T").build();
    // B<out T>
    auto typeSymbolB = ClassSymbolBuilder("B").with("T", Variance::COVARIANT).build();
    // C<in T>
    auto typeSymbolC = ClassSymbolBuilder("C").with("T", Variance::CONTRAVARIANT).build();

    auto typeAint = TypeBuilder(typeSymbolA).with(intType).build(); // A<int>
    auto typeAfloat = TypeBuilder(typeSymbolA).with(floatType).build(); // A<float>
    auto typeAnumber = TypeBuilder(typeSymbolA).with(numberType).build(); // A<number>

    auto typeBint = TypeBuilder(typeSymbolB).with(intType).build(); // B<int>
    auto typeBfloat = TypeBuilder(typeSymbolB).with(floatType).build(); // B<float>
    auto typeBnumber = TypeBuilder(typeSymbolB).with(numberType).build(); // B<number>

    auto typeCint = TypeBuilder(typeSymbolC).with(intType).build(); // C<int>
    auto typeCfloat = TypeBuilder(typeSymbolC).with(floatType).build(); // C<float>
    auto typeCnumber = TypeBuilder(typeSymbolC).with(numberType).build(); // C<number>

    // A is invariant in its generic parameter, so no subtyping
    testSubtypeRelations({typeAint, typeAfloat, typeAnumber}, {});

    // B is covariant so only B<int> <: B<number> and B<float> <: B<number> should hold
    testSubtypeRelations({typeBint, typeBfloat, typeBnumber}, {
        {typeBint, typeBnumber},
        {typeBfloat, typeBnumber}
    });

    // C is covariant so only C<number> <: C<int> and C<number> <: C<float> should hold
    testSubtypeRelations({typeCint, typeCfloat, typeCnumber}, {
        {typeCnumber, typeCint},
        {typeCnumber, typeCfloat}
    });
}

TEST_F(TypeTest, CorrectlyChecksSubtypingForChainOfInheritance) {
    // A <: B <: C
    auto typeSymbolC = ClassSymbolBuilder("C").build();
    auto typeSymbolB = ClassSymbolBuilder("B").withSuperClass(TypeBuilder(typeSymbolC).build()).build();
    auto typeSymbolA = ClassSymbolBuilder("A").withSuperClass(TypeBuilder(typeSymbolB).build()).build();

    auto typeC = TypeBuilder(typeSymbolC).build();
    auto typeB = TypeBuilder(typeSymbolB).build();
    auto typeA = TypeBuilder(typeSymbolA).build();

    testSubtypeRelations({typeA, typeB, typeC}, {
        {typeA, typeB},
        {typeB, typeC},
        {typeA, typeC}
    });
}

TEST_F(TypeTest, CorrectlyChecksSubtypingForDirectInheritanceWithGenericsHavingSingleParameter) {
    // B<T>
    auto typeSymbolB = ClassSymbolBuilder("B").with("T").build();
    // B<int>
    auto typeB1 = TypeBuilder(typeSymbolB).with(intType).build();
    // B<U>
    auto typeB2 = TypeBuilder(typeSymbolB).with(TypeBuilder("U").build()).build();

    // A<U>
    auto typeSymbolA = ClassSymbolBuilder("A").with("U").withSuperClass(typeB2).build();
    // A<int>
    auto typeA1 = TypeBuilder(typeSymbolA).with(intType).build();
    // A<U>
    auto typeA2 = TypeBuilder(typeSymbolA).with(TypeBuilder("U").build()).build();

    testSubtypeRelations({typeA1, typeA2, typeB1, typeB2}, {
        {typeA1, typeB1}, // A<int> <: B<int>
        {typeA2, typeB2}  // A<U> <: B<U>
    });

    // B<float>
    auto typeB3 = TypeBuilder(typeSymbolB).with(floatType).build();
    // B<V>
    auto typeB4 = TypeBuilder(typeSymbolB).with(TypeBuilder("V").build()).build();

    // False: A<int> <: B<float> (B's variance is invariant)
    // False: A<U> <: B<V> (unrelated generic types)
    testSubtypeRelations({typeA1, typeA2, typeB3, typeB4}, {});
}
