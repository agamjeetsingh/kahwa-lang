//
// Created by Agamjeet Singh on 04/12/25.
//

#include <gtest/gtest.h>

#include "../../include/diagnostics/DiagnosticEngine.h"
#include "../../include/parser/Parser.h"
#include "../../include/tokeniser/Tokeniser.h"
#include "../../include/parser/Modifier.h"
#include "../../include/parser/ASTBuilder.h"

class ParserTest : public testing::Test {
protected:
    inline static Arena astArena;
    DiagnosticEngine diagnostic_engine;
    Parser parser{astArena, diagnostic_engine};
    Tokeniser tokeniser{diagnostic_engine};

    void expectDiagnostics(const std::vector<Diagnostic>& diagnostics) {
        auto& actualDiagnostics = diagnostic_engine.getAll();
        EXPECT_EQ(actualDiagnostics.size(), alreadyExpected + diagnostics.size());
        EXPECT_TRUE((std::equal(actualDiagnostics.end() - diagnostics.size(), actualDiagnostics.end(), diagnostics.begin())));
        alreadyExpected += diagnostics.size();
    }

    void expectDiagnosticsIgnoreSourceRange(const std::vector<Diagnostic>& diagnostics) {
        auto& actualDiagnostics = diagnostic_engine.getAll();
        EXPECT_EQ(actualDiagnostics.size(), alreadyExpected + diagnostics.size());

        for (std::size_t i = alreadyExpected; i < actualDiagnostics.size(); i++) {
            EXPECT_EQ(actualDiagnostics[i].severity, diagnostics[i - alreadyExpected].severity);
            EXPECT_EQ(actualDiagnostics[i].kind, diagnostics[i - alreadyExpected].kind);
            EXPECT_EQ(actualDiagnostics[i].msg, diagnostics[i - alreadyExpected].msg);
        }
        alreadyExpected += diagnostics.size();
    }

    std::size_t alreadyExpected = 0;

    void expectNoDiagnostics() {
        expectDiagnostics({});
    }

    static inline SourceRange dummy_source = {0, 0};


    [[nodiscard]] KahwaFile* parseFile(const std::string &str) const {
        return parser.parseFile(tokeniser.tokenise(0, str));
    }

    static FieldDecl* createFieldDecl(const std::string& name,
                                      const std::vector<Modifier> &modifiers = {},
                                      TypeRef* type = nullptr) {
        return astArena.make<FieldDecl>(name, modifiers, type, dummy_source, dummy_source, dummy_source);
    }

    static bool declEqualIgnoreSourceRange(const Decl* d1, const Decl* d2) {
        if (d1 == nullptr && d2 == nullptr) return true;
        if (d1 == nullptr || d2 == nullptr) return false;
        return d1->name == d2->name && d1->modifiers == d2->modifiers;
    }

    static bool blockEqualIgnoreSourceRange(const Block* b1, const Block* b2) {
        if (b1 == nullptr && b2 == nullptr) return true;
        if (b1 == nullptr || b2 == nullptr) return false;
        if (b1->stmts.size() != b2->stmts.size()) return false;
        
        for (size_t i = 0; i < b1->stmts.size(); ++i) {
            if (b1->stmts[i] == nullptr && b2->stmts[i] == nullptr) continue;
            if (b1->stmts[i] == nullptr || b2->stmts[i] == nullptr) return false;
        }
        return true;
    }

    static bool typedefDeclEqualIgnoreSourceRange(const TypedefDecl* td1, const TypedefDecl* td2) {
        if (td1 == nullptr && td2 == nullptr) return true;
        if (td1 == nullptr || td2 == nullptr) return false;
        if (!declEqualIgnoreSourceRange(td1, td2)) return false;
        
        if (td1->referredType == nullptr && td2->referredType == nullptr) return true;
        if (td1->referredType == nullptr || td2->referredType == nullptr) return false;
        return *td1->referredType == *td2->referredType;
    }

    static bool fieldDeclEqualIgnoreSourceRange(const FieldDecl* fd1, const FieldDecl* fd2) {
        if (fd1 == nullptr && fd2 == nullptr) return true;
        if (fd1 == nullptr || fd2 == nullptr) return false;
        if (!declEqualIgnoreSourceRange(fd1, fd2)) return false;
        
        if (fd1->type == nullptr && fd2->type == nullptr) return true;
        if (fd1->type == nullptr || fd2->type == nullptr) return false;
        return *fd1->type == *fd2->type;
    }

    static bool methodDeclEqualIgnoreSourceRange(const MethodDecl* md1, const MethodDecl* md2) {
        if (md1 == nullptr && md2 == nullptr) return true;
        if (md1 == nullptr || md2 == nullptr) return false;
        if (!declEqualIgnoreSourceRange(md1, md2)) return false;
        
        if (md1->returnType == nullptr && md2->returnType == nullptr) {
            // Both null, continue
        } else if (md1->returnType == nullptr || md2->returnType == nullptr) {
            return false;
        } else if (*md1->returnType != *md2->returnType) {
            return false;
        }
        
        if (md1->parameters.size() != md2->parameters.size()) return false;
        for (size_t i = 0; i < md1->parameters.size(); ++i) {
            if (md1->parameters[i].second != md2->parameters[i].second) return false;
            
            auto &typeA = md1->parameters[i].first;
            auto &typeB = md2->parameters[i].first;
            if (typeA == nullptr && typeB == nullptr) continue;
            if (typeA == nullptr || typeB == nullptr) return false;
            if (*typeA != *typeB) return false;
        }
        
        return blockEqualIgnoreSourceRange(md1->block, md2->block);
    }

    static bool classDeclEqualIgnoreSourceRange(const ClassDecl* cd1, const ClassDecl* cd2) {
        if (cd1 == nullptr && cd2 == nullptr) return true;
        if (cd1 == nullptr || cd2 == nullptr) return false;
        if (!declEqualIgnoreSourceRange(cd1, cd2)) return false;
        
        if (cd1->superClasses.size() != cd2->superClasses.size() ||
            cd1->fields.size() != cd2->fields.size() ||
            cd1->methods.size() != cd2->methods.size() ||
            cd1->nestedClasses.size() != cd2->nestedClasses.size()) {
            return false;
        }
        
        for (size_t i = 0; i < cd1->superClasses.size(); ++i) {
            if (cd1->superClasses[i] == nullptr && cd2->superClasses[i] == nullptr) continue;
            if (cd1->superClasses[i] == nullptr || cd2->superClasses[i] == nullptr) return false;
            if (*cd1->superClasses[i] != *cd2->superClasses[i]) return false;
        }
        
        for (size_t i = 0; i < cd1->fields.size(); ++i) {
            if (!fieldDeclEqualIgnoreSourceRange(cd1->fields[i], cd2->fields[i])) return false;
        }
        
        for (size_t i = 0; i < cd1->methods.size(); ++i) {
            if (!methodDeclEqualIgnoreSourceRange(cd1->methods[i], cd2->methods[i])) return false;
        }
        
        for (size_t i = 0; i < cd1->nestedClasses.size(); ++i) {
            if (!classDeclEqualIgnoreSourceRange(cd1->nestedClasses[i], cd2->nestedClasses[i])) return false;
        }
        
        return true;
    }

    static bool kahwaFileEqualIgnoreSourceRange(const KahwaFile* kf1, const KahwaFile* kf2) {
        if (kf1 == nullptr && kf2 == nullptr) return true;
        if (kf1 == nullptr || kf2 == nullptr) return false;
        
        if (kf1->typedefDecls.size() != kf2->typedefDecls.size() ||
            kf1->classDecls.size() != kf2->classDecls.size() ||
            kf1->functionDecls.size() != kf2->functionDecls.size() ||
            kf1->variableDecls.size() != kf2->variableDecls.size()) {
            return false;
        }
        
        for (size_t i = 0; i < kf1->typedefDecls.size(); ++i) {
            if (!typedefDeclEqualIgnoreSourceRange(kf1->typedefDecls[i], kf2->typedefDecls[i])) return false;
        }
        
        for (size_t i = 0; i < kf1->classDecls.size(); ++i) {
            if (!classDeclEqualIgnoreSourceRange(kf1->classDecls[i], kf2->classDecls[i])) return false;
        }
        
        for (size_t i = 0; i < kf1->functionDecls.size(); ++i) {
            if (!methodDeclEqualIgnoreSourceRange(kf1->functionDecls[i], kf2->functionDecls[i])) return false;
        }
        
        for (size_t i = 0; i < kf1->variableDecls.size(); ++i) {
            if (!fieldDeclEqualIgnoreSourceRange(kf1->variableDecls[i], kf2->variableDecls[i])) return false;
        }
        
        return true;
    }

    static std::string toString(const std::vector<Modifier>& modifiers) {
        std::string str;
        for (int i = 0; i < modifiers.size(); i++) {
            str += ::toString(modifiers[i]);
            if (i != modifiers.size() - 1) str += " ";
        }

        return str;
    }

    static std::string toString(const TypeRef* type_ref) {
        std::string str = type_ref->identifier;
        if (!type_ref->args.empty()) {
            str += "<";

            for (int i = 0; i < type_ref->args.size(); i++) {
                str += toString(type_ref->args[i]);
                if (i != type_ref->args.size() - 1) {
                    str += ", ";
                }
            }

            str += ">";
        }

        return str;
    }

    static std::string toString(const TypedefDecl* typedefDecl) {
        std::string str = toString(typedefDecl->modifiers);
        if (!typedefDecl->modifiers.empty()) str += " ";
        str += "typedef " + toString(typedefDecl->referredType) + " " + typedefDecl->name + ";";
        return str;
    }

    static std::string toString(const ClassDecl* classDecl) {
        std::string str = toString(classDecl->modifiers);

        if (!classDecl->modifiers.empty()) str += " ";
        str += "class ";
        str += classDecl->name;
        if (!classDecl->superClasses.empty()) {
            str += ": ";

            for (int i = 0; i < classDecl->superClasses.size(); i++) {
                str += toString(classDecl->superClasses[i]);
                if (i != classDecl->superClasses.size() - 1) {
                    str += ", ";
                }
            }
        }

        str += " {\n";


        str += "}";

        return str;
    }
};

TEST_F(ParserTest, ParsesSingleTypedefCorrectly) {
    const auto typedefDecl1 = TypedefDeclBuilder("myInt", TypeRefBuilder("int").build()).build();
    const auto str1 = toString(typedefDecl1);
    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(str1), KahwaFileBuilder().with(typedefDecl1).build());

    const auto typedefDecl2 = TypedefDeclBuilder("SomeType", TypeRefBuilder("someOtherType").build()).with(Modifier::PRIVATE).build();
    const auto str2 = toString(typedefDecl2);
    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(str2), KahwaFileBuilder().with(typedefDecl2).build());

    const auto typedefDecl3 = TypedefDeclBuilder(
        "SomeTypeAgain",
        TypeRefBuilder("double_t20").build()
        ).with({Modifier::PUBLIC, Modifier::OPEN, Modifier::STATIC}).build();
    const auto str3 = toString(typedefDecl3);
    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(str3), KahwaFileBuilder().with(typedefDecl3).build());

    expectNoDiagnostics();
}

TEST_F(ParserTest, ParsesMultipleTypedefsCorrectly) {
    const auto typedefDecl1 = TypedefDeclBuilder("myInt", TypeRefBuilder("int").build()).build();
    const auto str1 = toString(typedefDecl1);

    const auto typedefDecl2 = TypedefDeclBuilder("SomeType", TypeRefBuilder("someOtherType").build()).with(Modifier::PRIVATE).build();
    const auto str2 = toString(typedefDecl2);

    const auto typedefDecl3 = TypedefDeclBuilder("SomeTypeAgain", TypeRefBuilder("double_t20").build()).with({Modifier::PUBLIC, Modifier::OPEN, Modifier::STATIC}).build();
    const auto str3 = toString(typedefDecl3);

    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(str1 + str2), KahwaFileBuilder().with({typedefDecl1, typedefDecl2}).build());
    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(str1 + str3), KahwaFileBuilder().with({typedefDecl1, typedefDecl3}).build());
    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(str2 + str3), KahwaFileBuilder().with({typedefDecl2, typedefDecl3}).build());
    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(str1 + str2 + str3), KahwaFileBuilder().with({typedefDecl1, typedefDecl2, typedefDecl3}).build());

    expectNoDiagnostics();
}

TEST_F(ParserTest, ParsesGenericTypesCorrectly) {
    const auto typedefDecl1 = TypedefDeclBuilder(
        "customType",
        TypeRefBuilder("vector")
        .with(TypeRefBuilder("int").build())
        .build())
    .build();
    const auto str1 = toString(typedefDecl1);

    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(str1), KahwaFileBuilder().with(typedefDecl1).build());

    const auto typedefDecl2 = TypedefDeclBuilder(
        "multipleArgs",
        TypeRefBuilder("variant")
        .with(TypeRefBuilder("arg1").build())
        .with(TypeRefBuilder("arg2").build())
        .with(TypeRefBuilder("arg3").build())
        .build())
    .build();
    const auto str2 = toString(typedefDecl2);

    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(str2), KahwaFileBuilder().with(typedefDecl2).build());

    const auto typedefDecl3 = TypedefDeclBuilder(
        "nested",
        TypeRefBuilder("vector")
        .with(TypeRefBuilder("pair")
            .with(TypeRefBuilder("int").build())
            .with(TypeRefBuilder("bool").build())
            .build())
        .build())
    .build();
    const auto str3 = toString(typedefDecl3);

    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(str3), KahwaFileBuilder().with(typedefDecl3).build());

    const auto typedefDecl4 = TypedefDeclBuilder(
        "nestedWithMoreArgs",
        TypeRefBuilder("vector")
        .with(TypeRefBuilder("pair")
            .with(TypeRefBuilder("pair")
                .with(TypeRefBuilder("int").build())
                .with(TypeRefBuilder("bool").build())
                .build())
            .with(TypeRefBuilder("bool").build())
            .build())
        .build())
    .build();
    const auto str4 = toString(typedefDecl4);

    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(str4), KahwaFileBuilder().with(typedefDecl4).build());

    expectNoDiagnostics();
}

TEST_F(ParserTest, ReportsCorrectDiagnosticWhenTypedefIsMalformed) {
    const auto typedefDecl = TypedefDeclBuilder("myInt", TypeRefBuilder("int").build()).build();
    const std::string str1 = "typedef int myInt;";
    const std::string str2 = "typedef int myInt";
    const std::string str3 = "typedef 0 0";
    const std::string str4 = "typedef int 0";
    const std::string str5 = "typedef";

    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(str1), KahwaFileBuilder().with(typedefDecl).build());

    expectNoDiagnostics();

    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(str2), KahwaFileBuilder().build());

    Diagnostic expectedSemicolon = {DiagnosticSeverity::ERROR, DiagnosticKind::EXPECTED_SEMI_COLON, dummy_source};
    expectDiagnosticsIgnoreSourceRange({expectedSemicolon});

    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(str3), KahwaFileBuilder().build());

    Diagnostic expectedIdentifier = {DiagnosticSeverity::ERROR, DiagnosticKind::EXPECTED_IDENTIFIER, dummy_source};
    expectDiagnosticsIgnoreSourceRange({ expectedIdentifier});

    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(str4), KahwaFileBuilder().build());
    expectDiagnosticsIgnoreSourceRange({ expectedIdentifier});

    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(str5), KahwaFileBuilder().build());
    expectDiagnosticsIgnoreSourceRange({expectedIdentifier});

    const std::string str6 = "typedef 0 int";
    Diagnostic expectedDeclaration = {DiagnosticSeverity::ERROR, DiagnosticKind::EXPECTED_DECLARATION, dummy_source};

    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(str6), KahwaFileBuilder().build());
    expectDiagnosticsIgnoreSourceRange({expectedIdentifier, expectedDeclaration});
}

TEST_F(ParserTest, RecoversFromMalformedTypedefCorrectly) {
    const std::string str1 = " typedef int myInt ";
    const std::string str2 = " typedef 0 0 ";
    const std::string str3 = " typedef int 0 ";
    const auto typedefDecl = TypedefDeclBuilder("workNow", TypeRefBuilder("should").build()).build();
    const auto validStr = toString(typedefDecl);

    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(str1 + validStr), KahwaFileBuilder().with(typedefDecl).build());
    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(str2 + validStr), KahwaFileBuilder().with(typedefDecl).build());
    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(str3 + validStr), KahwaFileBuilder().with(typedefDecl).build());
    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(validStr + str1), KahwaFileBuilder().with(typedefDecl).build());
    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(validStr + str1 + str2 + str3), KahwaFileBuilder().with(typedefDecl).build());
    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(validStr + str1 + validStr + str2), KahwaFileBuilder().with({typedefDecl, typedefDecl}).build());
    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(validStr + str1 + validStr + str2 + str3 + validStr), KahwaFileBuilder().with({3, typedefDecl}).build());

    const std::string str4 = " private typedef ";
    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(str4 + validStr), KahwaFileBuilder().with(typedefDecl).build());
}

TEST_F(ParserTest, ParsesEmptyClassCorrectly) {
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

TEST_F(ParserTest, ParsesEmptyClassWithInheritanceCorrectly) {
    const auto classDecl1 = ClassDeclBuilder("className")
    .with(TypeRefBuilder("superClass").build())
    .build();
    const auto str1 = toString(classDecl1);

    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(str1), KahwaFileBuilder().with(classDecl1).build());

    const auto classDecl2 = ClassDeclBuilder("className")
    .with({TypeRefBuilder("superClass1").build(), TypeRefBuilder("superClass2").build()})
    .build();
    const auto str2 = toString(classDecl2);

    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(str2), KahwaFileBuilder().with(classDecl2).build());

    const auto classDecl3 = ClassDeclBuilder("className")
    .with({TypeRefBuilder("superClass1").build(), TypeRefBuilder("superClass2").build()})
    .with(Modifier::FINAL)
    .with(Modifier::PRIVATE)
    .build();
    const auto str3 = toString(classDecl3);

    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(str3), KahwaFileBuilder().with(classDecl3).build());

    const auto classDecl4 = ClassDeclBuilder("className")
    .with(TypeRefBuilder("superClass1")
        .with({
            TypeRefBuilder("arg1").build(),
            TypeRefBuilder("arg2").build()
        }).build())
    .with(Modifier::FINAL)
    .with(Modifier::PRIVATE)
    .build();
    const auto str4 = toString(classDecl4);

    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(str4), KahwaFileBuilder().with(classDecl4).build());

    expectNoDiagnostics();
}

TEST_F(ParserTest, ParsesMultipleClassesWithInheritanceAndTypeDefsCorrectly) {
    const auto classDecl1 = ClassDeclBuilder("className")
        .with({TypeRefBuilder("superClass1").build(), TypeRefBuilder("superClass2").build()})
        .with(Modifier::FINAL)
        .with(Modifier::PRIVATE)
        .build();
    const auto str1 = toString(classDecl1);

    const auto classDecl2 = ClassDeclBuilder("className")
    .with(TypeRefBuilder("superClass1")
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

    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(str1 + str2 + str3),
        KahwaFileBuilder()
        .with({classDecl1, classDecl2})
        .with({typedefDecl1, typedefDecl2})
        .build());

    expectNoDiagnostics();
}

TEST_F(ParserTest, ParsesClassesWithMethodsCorrectly) {
    const auto classDecl1 = ClassDeclBuilder("className")
    .with(TypeRefBuilder("superClass1")
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
    .with(TypeRefBuilder("superClass1")
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

TEST_F(ParserTest, ParsesClassesWithNestedClassesCorrectly) {
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
        .with(TypeRefBuilder("superClass1")
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
