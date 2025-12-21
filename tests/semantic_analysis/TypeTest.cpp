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

TEST_F(TypeTest, CorrectlyChecksSubtypingForGenericVarianceWithMultipleParameters) {
    // A<T, in U, out T>
    auto typeSymbolA = ClassSymbolBuilder("A").with(
        {"T", "U", "V"},
        {Variance::INVARIANT, Variance::CONTRAVARIANT, Variance::COVARIANT}).build();

    std::vector types = {
        TypeBuilder(typeSymbolA) // 0. A<T, int, number>
        .with({TypeBuilder("T").build(), intType, numberType}).build(),
        TypeBuilder(typeSymbolA) // 1. A<T, number, number>
        .with({TypeBuilder("T").build(), numberType, numberType}).build(),
        TypeBuilder(typeSymbolA) // 2. A<T, int, float>
        .with({TypeBuilder("T").build(), intType, floatType}).build(),
        TypeBuilder(typeSymbolA) // 3. A<T, number, float>
        .with({TypeBuilder("T").build(), numberType, floatType}).build(),
        TypeBuilder(typeSymbolA) // 4. A<float, number, number>
        .with({floatType, numberType, numberType}).build(),
        TypeBuilder(typeSymbolA) // 5. A<float, int, number>
        .with({floatType, intType, numberType}).build(),
        TypeBuilder(typeSymbolA) // 6. A<float, number, float>
        .with({floatType, numberType, floatType}).build(),
        TypeBuilder(typeSymbolA) // 7. A<number, number, float>
        .with({numberType, numberType, floatType}).build()
    };

    testSubtypeRelations(types, {
        {types[1], types[0]},
        {types[2], types[0]},
        {types[3], types[0]},
        {types[3], types[1]},
        {types[3], types[2]},
        {types[4], types[5]},
        {types[6], types[4]},
        {types[6], types[5]}
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

TEST_F(TypeTest, CorrectlyChecksSubtypingForDirectInheritanceWithGenericsHavingMultipleParameters) {
    // C<T, U, V>
    auto typeSymbolC = ClassSymbolBuilder("C")
    .with("T")
    .with("U")
    .with("V")
    .build();
    // C<T, U, V>
    auto typeC1 = TypeBuilder(typeSymbolC).with({TypeBuilder("T").build(), TypeBuilder("U").build(), TypeBuilder("V").build()}).build();
    // C<int, float, int>
    auto typeC2 = TypeBuilder(typeSymbolC).with({intType, floatType, intType}).build();
    // C<int, U, float>
    auto typeC3 = TypeBuilder(typeSymbolC).with({intType, TypeBuilder("U").build(), floatType}).build();

    // B<V, T> <: A<T, U, V>
    auto typeSymbolB = ClassSymbolBuilder("B")
    .with("V")
    .with("T")
    .withSuperClass(TypeBuilder(typeSymbolC).with(typeC1).build())
    .build();

    // auto typeSymbolC = ClassSymbolBuilder("C")
    // .with("T")
    // .with("U")
    // .withSuperClass()
}

// ======= TESTING TYPE SUBSTITUTION =======

// substitute(A<T, U, V>, B<C<T>, V>, A<int, int, float>) = B<C<int>, float>
// substitute(A<T, U, V>, B<T, V>, A<T, int, float>) = B<T, float>
// substitute(A<T, U, V>, B<T>, A<int, int, float>) = B<int>
// substitute(A<T, U, V>, T, A<int, int, float>) = int
// substitute(A<T, U, V>, double, A<int, int, float>) = double
TEST_F(TypeTest, CorrectlySubstitutesTypes) {
    // A<T, U, V>
    auto typeSymbolA = ClassSymbolBuilder("A").with({"T", "U", "V"}).build();

    // Test 1: A<T, U, V>, float, A<int, int, float> --> float
    auto typeA1 = TypeBuilder(typeSymbolA).with({intType, intType, floatType}).build();
    ASSERT_TRUE((*Type::substitute(typeSymbolA->genericArguments, floatType, typeA1) == *floatType));

    // Test 2: A<T, U, V>, T, A<int, int, float> --> int
    auto typeA2 = TypeBuilder(typeSymbolA).with({intType, intType, floatType}).build();
    ASSERT_TRUE((*Type::substitute(typeSymbolA->genericArguments, TypeBuilder("T").build(), typeA2) == *intType));

    // Test 3: A<T, U, V>, B<T>, A<int, int, float> --> B<int>
    auto typeA3 = TypeBuilder(typeSymbolA).with({intType, intType, floatType}).build();
    auto typeSymbolB3 = ClassSymbolBuilder("B").withNumOfGenericArgs(1).build();
    auto typeB3 = TypeBuilder(typeSymbolB3).with(TypeBuilder("T").build()).build();
    auto typeRes3 = TypeBuilder(typeSymbolB3).with(intType).build();
    ASSERT_TRUE((*Type::substitute(typeSymbolA->genericArguments, typeB3, typeA3) == *typeRes3));

    // Test 4: A<T, U, V>, B<T, V>, A<T, int, float> --> B<T, float>
    auto typeA4 = TypeBuilder(typeSymbolA).with({TypeBuilder("T").build(), intType, floatType}).build();
    auto typeSymbolB4 = ClassSymbolBuilder("B").withNumOfGenericArgs(2).build();
    auto typeB4 = TypeBuilder(typeSymbolB4).with({
        TypeBuilder("T").build(),
        TypeBuilder("V").build()
        }).build();
    auto typeRes4 = TypeBuilder(typeSymbolB4).with({
        TypeBuilder("T").build(),
        floatType
    }).build();

    ASSERT_TRUE((*Type::substitute(typeSymbolA->genericArguments, typeB4, typeA4) == *typeRes4));

    // Test 5: A<T, U, V>, B<C<T>, V, U>, A<int, float, T> --> B<C<int>, T, float>
    auto typeA5 = TypeBuilder(typeSymbolA).with({intType, floatType, TypeBuilder("T").build()}).build();
    auto typeSymbolB5 = ClassSymbolBuilder("B").withNumOfGenericArgs(3).build();
    auto typeSymbolC5 = ClassSymbolBuilder("C").withNumOfGenericArgs(1).build();
    auto typeB5 = TypeBuilder(typeSymbolB5).with({
        TypeBuilder(typeSymbolC5).with(TypeBuilder("T").build()).build(),
        TypeBuilder("V").build(),
        TypeBuilder("U").build()}).build();
    auto typeRes5 = TypeBuilder(typeSymbolB5)
    .with(TypeBuilder(typeSymbolC5).with(intType).build())
    .with(TypeBuilder("T").build())
    .with(floatType)
    .build();

    ASSERT_TRUE((*Type::substitute(typeSymbolA->genericArguments, typeB5, typeA5) == *typeRes5));
}
