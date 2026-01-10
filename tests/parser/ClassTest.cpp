//
// Created by Agamjeet Singh on 28/12/25.
//

#include <gtest/gtest.h>
#include "ParserTestBase.cpp"

class ClassTest : public testing::Test, public ParserTestBase {
protected:
    void testClasses(const std::vector<ClassDecl*>& classes) const {
        for (auto* expectedClass: classes) {
            auto parsedClass = parseClass(toString(expectedClass));
            EXPECT_PRED2(classDeclEqualIgnoreSourceRange, parsedClass, expectedClass);
            std::cout << "parsedClass: " << toString(parsedClass) << "\nexpectedClass: " << toString(expectedClass) << "\n--------" << std::endl;
        }
    }
};

TEST_F(ClassTest, ParsesEmptyClassCorrectly) {
    const auto classDecl1 = ClassDeclBuilder("myClass").build();
    const auto str1 = toString(classDecl1);

    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(str1), KahwaFileBuilder().with(classDecl1).build());

    const auto classDecl2 = ClassDeclBuilder("myClass")
    .with(Modifier::PRIVATE)
    .build();
    const auto str2 = toString(classDecl2);

    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(str2), KahwaFileBuilder().with(classDecl2).build());

    const auto classDecl3 = ClassDeclBuilder("classNamesCanHaveNumbers")
    .with(Modifier::PRIVATE)
    .with(Modifier::FINAL)
    .build();
    const auto str3 = toString(classDecl3);

    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(str3), KahwaFileBuilder().with(classDecl3).build());

    expectNoDiagnostics();
}

TEST_F(ClassTest, ParsesEmptyClassWithInheritanceCorrectly) {
    const auto classDecl1 = ClassDeclBuilder("className")
    .withSuperClass(TypeRefBuilder("superClass").build())
    .build();
    const auto str1 = toString(classDecl1);

    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(str1), KahwaFileBuilder().with(classDecl1).build());

    const auto classDecl2 = ClassDeclBuilder("className")
    .withSuperClasses({TypeRefBuilder("superClass1").build(), TypeRefBuilder("superClass2").build()})
    .build();
    const auto str2 = toString(classDecl2);

    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(str2), KahwaFileBuilder().with(classDecl2).build());

    const auto classDecl3 = ClassDeclBuilder("className")
    .withSuperClasses({TypeRefBuilder("superClass1").build(), TypeRefBuilder("superClass2").build()})
    .with(Modifier::FINAL)
    .with(Modifier::PRIVATE)
    .build();
    const auto str3 = toString(classDecl3);

    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(str3), KahwaFileBuilder().with(classDecl3).build());

    const auto classDecl4 = ClassDeclBuilder("className")
    .withSuperClass(TypeRefBuilder("superClass1")
        .with({
            TypeRefBuilder("arg1").build(),
            TypeRefBuilder("arg2").build()
        }).build())
    .withSuperClass(TypeRefBuilder("superClass2").build())
    .with(Modifier::FINAL)
    .with(Modifier::PRIVATE)
    .build();
    const auto str4 = toString(classDecl4);

    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(str4), KahwaFileBuilder().with(classDecl4).build());

    expectNoDiagnostics();
}

TEST_F(ClassTest, ParsesMultipleClassesWithInheritanceAndTypeDefsCorrectly) {
    const auto classDecl1 = ClassDeclBuilder("className")
        .withSuperClasses({TypeRefBuilder("superClass1").build(), TypeRefBuilder("superClass2").build()})
        .with(Modifier::FINAL)
        .with(Modifier::PRIVATE)
        .build();
    const auto str1 = toString(classDecl1);

    const auto classDecl2 = ClassDeclBuilder("className")
    .withSuperClass(TypeRefBuilder("superClass1")
        .with({
            TypeRefBuilder("arg1").build(),
            TypeRefBuilder("arg2").build()
        }).build())
    .with(Modifier::FINAL)
    .with(Modifier::PRIVATE)
    .build();
    const auto str2 = toString(classDecl2);

    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(str1 + str2), KahwaFileBuilder().with({classDecl1, classDecl2}).build());

    const auto typedefDecl1 = TypedefDeclBuilder("someTypeDef", TypeRefBuilder("int").build()).build();
    const auto str3 = toString(typedefDecl1);

    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(str1 + str2 + str3),
        KahwaFileBuilder()
        .with({classDecl1, classDecl2})
        .with(typedefDecl1)
        .build());

    const auto typedefDecl2 = TypedefDeclBuilder("anotherTypeDef", TypeRefBuilder("float").build()).build();
    const auto str4 = toString(typedefDecl2);

    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(str1 + str2 + str3 + str4),
        KahwaFileBuilder()
        .with({classDecl1, classDecl2})
        .with({typedefDecl1, typedefDecl2})
        .build());

    expectNoDiagnostics();
}

TEST_F(ClassTest, ParsesClassesWithMethodsWithEmptyBodiesCorrectly) {
    const auto classDecl1 = ClassDeclBuilder("className")
    .withSuperClass(TypeRefBuilder("superClass1")
        .with(TypeRefBuilder("arg1").build())
        .build())
    .with(Modifier::FINAL)
    .with(MethodDeclBuilder(
        "foo",
        TypeRefBuilder("int").build(),
        BlockBuilder().build()
        ).build())
    .build();
    const auto str1 = toString(classDecl1);

    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(str1),
        KahwaFileBuilder()
        .with(classDecl1)
        .build());

    const auto classDecl2 = ClassDeclBuilder("className")
    .withSuperClass(TypeRefBuilder("superClass1")
        .with(TypeRefBuilder("arg1").build())
        .build())
    .with(Modifier::FINAL)
    .with(MethodDeclBuilder(
        "foo",
        TypeRefBuilder("int").build(),
        BlockBuilder().build())
        .with(Modifier::PRIVATE)
        .with(Modifier::STATIC)
        .with({TypeRefBuilder("int").build(), "length"})
        .with({TypeRefBuilder("vector") // vector<int>
            .with(TypeRefBuilder("int").build())
            .build(), "list"})
        .build())
    .build();
    const auto str2 = toString(classDecl2);

    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(str2),
        KahwaFileBuilder()
        .with(classDecl2)
        .build());

    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(str1 + str2),
        KahwaFileBuilder()
        .with(classDecl1)
        .with(classDecl2)
        .build());

    expectNoDiagnostics();
}

TEST_F(ClassTest, ParsesClassesWithNestedClassesCorrectly) {
    const auto classDecl1 = ClassDeclBuilder("ExampleClass")
    .with(Modifier::PRIVATE)
    .with(ClassDeclBuilder("NestedClass").build())
    .build();
    const auto str1 = toString(classDecl1);

    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(str1),
        KahwaFileBuilder()
        .with(classDecl1)
        .build());

    const auto classDecl2 = ClassDeclBuilder("AnotherClass")
    .with(Modifier::PRIVATE)
    .with(ClassDeclBuilder("NestedClass1").build())
    .with(ClassDeclBuilder("NestedClass2").build())
    .build();
    const auto str2 = toString(classDecl2);

    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(str2),
        KahwaFileBuilder()
        .with(classDecl2)
        .build());

    const auto classDecl3 = ClassDeclBuilder("AnotherOneWow")
    .with(ClassDeclBuilder("className")
        .withSuperClass(TypeRefBuilder("superClass1")
        .with({
            TypeRefBuilder("arg1").build(),
            TypeRefBuilder("arg2").build()
        }).build())
        .with(Modifier::FINAL)
        .with(Modifier::PROTECTED)
        .build())
    .with(ClassDeclBuilder("JustAnotherNestedClass")
        .with(ClassDeclBuilder("OhItGoesDeeper")
            .with(ClassDeclBuilder("ThisIsTheEnd")
                .build())
            .build())
        .build())
    .build();
    const auto str3 = toString(classDecl3);

    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(str3),
        KahwaFileBuilder()
        .with(classDecl3)
        .build());

    expectNoDiagnostics();
}

TEST_F(ClassTest, ParsesClassWithTypeParametersCorrectly) {
    std::vector classDecls = {
        ClassDeclBuilder("MyClass") // MyClass<T>
        .withTypeParameters({"T"})
        .build(),
        ClassDeclBuilder("MyClass") // MyClass<T, U>
        .withTypeParameters({"T", "U"})
        .build(),
        ClassDeclBuilder("MyClass") // MyClass<T, U, V>
        .withTypeParameters({"T", "U", "V"})
        .build(),
        ClassDeclBuilder("MyClass") // MyClass<Multilength, should, be, okay, too>
        .withTypeParameters({"Multilength", "should", "be", "okay", "too"})
        .build(),
    };

    std::vector<std::string> strs{classDecls.size()};
    std::ranges::transform(classDecls, strs.begin(), [](const ClassDecl* classDecl){ return toString(classDecl); });

    for (int i = 0; i < classDecls.size(); i++) {
        EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(strs[i]),
            KahwaFileBuilder()
            .with(classDecls[i])
            .build());
    }

    expectNoDiagnostics();
}

TEST_F(ClassTest, ParsesClassWithTypeParametersAndVarianceCorrectly) {
    std::vector classDecls = {
        ClassDeclBuilder("MyClass") // MyClass<in T>
        .withTypeParameters({"T"}, {Variance::CONTRAVARIANT})
        .build(),
        ClassDeclBuilder("MyClass") // MyClass<out T, in U>
        .withTypeParameters({"T", "U"}, {Variance::COVARIANT, Variance::CONTRAVARIANT})
        .build(),
        ClassDeclBuilder("MyClass") // MyClass<in T, U, out V>
        .withTypeParameters({"T", "U", "V"}, {Variance::CONTRAVARIANT, Variance::INVARIANT, Variance::COVARIANT})
        .build(),
        ClassDeclBuilder("MyClass") // MyClass<in Multilength, in should, in be, in okay, in too>
        .withTypeParameters({"Multilength", "should", "be", "okay", "too"}, std::vector(5, Variance::CONTRAVARIANT))
        .build(),
    };

    std::vector<std::string> strs{classDecls.size()};
    std::ranges::transform(classDecls, strs.begin(), [](const ClassDecl* classDecl){ return toString(classDecl); });

    for (int i = 0; i < classDecls.size(); i++) {
        if (!kahwaFileEqualIgnoreSourceRange(parseFile(strs[i]), KahwaFileBuilder()
            .with(classDecls[i])
            .build())) {
        }

        EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(strs[i]),
            KahwaFileBuilder()
            .with(classDecls[i])
            .build());
    }

    expectNoDiagnostics();
}

TEST_F(ClassTest, ParsesClassesWithFieldsCorrectly) {
    std::vector<ClassDecl*> classes;

    std::vector fields = {
        FieldDeclBuilder("bar1", TypeRefBuilder("bool").build()).build(),
        FieldDeclBuilder("bar2", TypeRefBuilder("int").build()).with(Modifier::PRIVATE).build(),
        FieldDeclBuilder("bar3", TypeRefBuilder("int").build()).with(Modifier::ABSTRACT).build()
    };

    for (int i = 0; i < fields.size(); i++) {
        std::vector<FieldDecl*> fieldSubset{fields.begin(), fields.begin() + i + 1};

        classes.push_back(ClassDeclBuilder("foo").with(fieldSubset).build());
    }

    testClasses(classes);
    expectNoDiagnostics();
}