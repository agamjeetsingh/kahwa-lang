//
// Created by Agamjeet Singh on 04/12/25.
//

#include <gtest/gtest.h>

#include "../../include/diagnostics/DiagnosticEngine.h"
#include "../../include/parser/Parser.h"
#include "../../include/tokeniser/Tokeniser.h"
#include "../../include/parser/Modifier.h"

class ParserTest : public testing::Test {
protected:
    inline static Arena astArena;
    DiagnosticEngine diagnostic_engine;
    Parser parser{astArena, diagnostic_engine};
    Tokeniser tokeniser{diagnostic_engine};

    void expectDiagnostics(const std::vector<Diagnostic>& diagnostics) const {
        EXPECT_EQ(diagnostic_engine.getAll(), diagnostics);
    }

    void expectNoDiagnostics() const {
        expectDiagnostics({});
    }

    [[nodiscard]] KahwaFile* parseFile(const std::string &str) const {
        return parser.parseFile(tokeniser.tokenise(0, str));
    }

    inline static SourceRange dummy_source = {0, 0};

    static KahwaFile* createKahwaFile(const std::vector<TypedefDecl*> &typedefDecls = {},
                                      const std::vector<ClassDecl*> &classDecls = {},
                                      const std::vector<MethodDecl*> &functionDecls = {},
                                      const std::vector<FieldDecl*> &variableDecls = {}) {
        return astArena.make<KahwaFile>(typedefDecls, classDecls, functionDecls, variableDecls);
    }

    class KahwaFileBuilder {
    public:
        KahwaFileBuilder() = default;

        KahwaFile* build() const {
            return createKahwaFile(typedefDecls, classDecls, functionDecls, variableDecls);
        }

        KahwaFileBuilder& with(TypedefDecl* typedefDecl) {
            typedefDecls.push_back(typedefDecl);
            return *this;
        }

        KahwaFileBuilder& with(std::vector<TypedefDecl*> typedefDecls) {
            this->typedefDecls.insert(this->typedefDecls.end(), typedefDecls.begin(), typedefDecls.end());
            return *this;
        }

        KahwaFileBuilder& with(ClassDecl* classDecl) {
            classDecls.push_back(classDecl);
            return *this;
        }

        KahwaFileBuilder& with(std::vector<ClassDecl*> classDecls) {
            this->classDecls.insert(this->classDecls.end(), classDecls.begin(), classDecls.end());
            return *this;
        }

        KahwaFileBuilder& with(MethodDecl* functionDecl) {
            functionDecls.push_back(functionDecl);
            return *this;
        }

        KahwaFileBuilder& with(std::vector<MethodDecl*> functionDecls) {
            this->functionDecls.insert(this->functionDecls.end(), functionDecls.begin(), functionDecls.end());
            return *this;
        }

        KahwaFileBuilder& with(FieldDecl* variableDecl) {
            variableDecls.push_back(variableDecl);
            return *this;
        }

        KahwaFileBuilder& with(std::vector<FieldDecl*> variableDecls) {
            this->variableDecls.insert(this->variableDecls.end(), variableDecls.begin(), variableDecls.end());
            return *this;
        }

    private:
        std::vector<TypedefDecl*> typedefDecls;
        std::vector<ClassDecl*> classDecls;
        std::vector<MethodDecl*> functionDecls;
        std::vector<FieldDecl*> variableDecls;
    };

    static TypedefDecl* createTypedefDecl(const std::string &name,
                                          TypeRef *referredType,
                                          const std::vector<Modifier> &modifiers = {}) {
        return astArena.make<TypedefDecl>(name, modifiers, referredType, dummy_source, dummy_source, dummy_source);
    }

    class TypeRefBuilder {
    public:
        explicit TypeRefBuilder(const std::string& name): name(name) {}

        TypeRef* build() const {
            return createTypeRef(name, typeRefs);
        }

        TypeRefBuilder& with(TypeRef* typeRef) {
            typeRefs.push_back(typeRef);
            return *this;
        }

        TypeRefBuilder& with(std::vector<TypeRef*> typeRefs) {
            this->typeRefs.insert(this->typeRefs.end(), typeRefs.begin(), typeRefs.end());
            return *this;
        }

    private:
        std::string name;
        std::vector<TypeRef*> typeRefs;
    };

    static TypeRef* createTypeRef(const std::string& identifier,
                                  const std::vector<TypeRef*> &args = {}) {
        return astArena.make<TypeRef>(identifier, args);
    }

    static FieldDecl* createFieldDecl(const std::string& name,
                                      const std::vector<Modifier> &modifiers = {},
                                      TypeRef* type = nullptr) {
        return astArena.make<FieldDecl>(name, modifiers, type, dummy_source, dummy_source, dummy_source);
    }

    class MethodDeclBuilder {
    public:
        MethodDeclBuilder(const std::string& name, TypeRef* returnType, Block* block)
            : name(name), returnType(returnType), block(block) {}

        MethodDecl* build() const {
            return createMethodDecl(name, modifiers, returnType, parameters, block);
        }

        MethodDeclBuilder& with(Modifier modifier) {
            modifiers.push_back(modifier);
            return *this;
        }

        MethodDeclBuilder& with(const std::vector<Modifier>& modifiers) {
            this->modifiers.insert(this->modifiers.begin(), modifiers.begin(), modifiers.end());
            return *this;
        }

        MethodDeclBuilder& with(const std::pair<TypeRef*, std::string> &parameter) {
            parameters.push_back(parameter);
            return *this;
        }

        MethodDeclBuilder& with(const std::vector<std::pair<TypeRef*, std::string>>& parameters) {
            this->parameters.insert(this->parameters.begin(), parameters.begin(), parameters.end());
            return *this;
        }

    private:
        std::string name;
        std::vector<Modifier> modifiers;
        TypeRef* returnType;
        std::vector<std::pair<TypeRef*, std::string>> parameters;
        Block* block;
    };

    static MethodDecl* createMethodDecl(const std::string& name,
                                        const std::vector<Modifier> &modifiers = {},
                                        TypeRef* returnType = nullptr,
                                        const std::vector<std::pair<TypeRef*, std::string>>& parameters = {},
                                        Block* block = nullptr) {
        return astArena.make<MethodDecl>(name, modifiers, returnType, parameters, block, dummy_source, dummy_source, dummy_source);
    }

     class BlockBuilder {
        public:

        BlockBuilder() = default;

        Block* build() const {
            return createBlock(stmts);
        }

        BlockBuilder& with(Stmt* stmt) {
            stmts.push_back(stmt);
            return *this;
        }

        BlockBuilder& with(const std::vector<Stmt*> &stmts) {
            this->stmts.insert(this->stmts.begin(), stmts.begin(), stmts.end());
            return *this;
        }

        private:
        std::vector<Stmt*> stmts;
    };

    static Block* createBlock(const std::vector<Stmt*>& stmts = {}) {
        return astArena.make<Block>(stmts);
    }

    class ClassDeclBuilder {
    public:
        explicit ClassDeclBuilder(const std::string& name): name(name) {}

        ClassDecl* build() const {
            return createClassDecl(name, modifiers, superClasses, fields, methods, nestedClasses);
        }

        ClassDeclBuilder& with(Modifier modifier) {
            modifiers.push_back(modifier);
            return *this;
        }

        ClassDeclBuilder& with(std::vector<Modifier> modifiers) {
            this->modifiers.insert(this->modifiers.end(), modifiers.begin(), modifiers.end());
            return *this;
        }

        ClassDeclBuilder& with(TypeRef* superClass) {
            superClasses.push_back(superClass);
            return *this;
        }

        ClassDeclBuilder& with(std::vector<TypeRef*> superClasses) {
            this->superClasses.insert(this->superClasses.end(), superClasses.begin(), superClasses.end());
            return *this;
        }

        ClassDeclBuilder& with(FieldDecl* field) {
            fields.push_back(field);
            return *this;
        }

        ClassDeclBuilder& with(std::vector<FieldDecl*> fields) {
            this->fields.insert(this->fields.end(), fields.begin(), fields.end());
            return *this;
        }

        ClassDeclBuilder& with(MethodDecl* method) {
            methods.push_back(method);
            return *this;
        }

        ClassDeclBuilder& with(std::vector<MethodDecl*> methods) {
            this->methods.insert(this->methods.end(), methods.begin(), methods.end());
            return *this;
        }

        ClassDeclBuilder& with(ClassDecl* nestedClass) {
            nestedClasses.push_back(nestedClass);
            return *this;
        }

        ClassDeclBuilder& with(std::vector<ClassDecl*> nestedClasses) {
            this->nestedClasses.insert(this->nestedClasses.end(), nestedClasses.begin(), nestedClasses.end());
            return *this;
        }

    private:
        std::string name;
        std::vector<Modifier> modifiers;
        std::vector<TypeRef*> superClasses;
        std::vector<FieldDecl*> fields;
        std::vector<MethodDecl*> methods;
        std::vector<ClassDecl*> nestedClasses;
    };

    static ClassDecl* createClassDecl(const std::string& name,
                                      const std::vector<Modifier> &modifiers = {},
                                      const std::vector<TypeRef*>& superClasses = {},
                                      const std::vector<FieldDecl*> &fields = {},
                                      const std::vector<MethodDecl*> &methods = {},
                                      const std::vector<ClassDecl*> &nestedClasses = {}) {
        return astArena.make<ClassDecl>(name, dummy_source, dummy_source, dummy_source, modifiers, superClasses, fields, methods, nestedClasses);
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

    static std::pair<std::string, TypedefDecl*> createSimpleTypeDef(const std::string& name, const std::vector<Modifier>& modifiers, const std::string& typeName) {
        auto decl = createTypedefDecl(name, createTypeRef(typeName), modifiers);
        std::string str = toString(modifiers);
        if (!modifiers.empty()) str += " ";
        str += "typedef " + typeName + " " + name + ";";
        return {str, decl};
    }
};

TEST_F(ParserTest, ParsesSingleTypedefCorrectly) {
    const auto typedefDecl1 = createTypedefDecl("myInt", createTypeRef("int"));
    const auto str1 = toString(typedefDecl1);
    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(str1), createKahwaFile({typedefDecl1}));

    const auto typedefDecl2 = createTypedefDecl("SomeType", createTypeRef("someOtherType"), {Modifier::PRIVATE});
    const auto str2 = toString(typedefDecl2);
    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(str2), createKahwaFile({typedefDecl2}));

    const auto typedefDecl3 = createTypedefDecl("SomeTypeAgain", createTypeRef("double_t20"), {Modifier::PUBLIC, Modifier::OPEN, Modifier::STATIC});
    const auto str3 = toString(typedefDecl3);
    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(str3), createKahwaFile({typedefDecl3}));

    expectNoDiagnostics();
}

TEST_F(ParserTest, ParsesMultipleTypedefsCorrectly) {
    const auto typedefDecl1 = createTypedefDecl("myInt", createTypeRef("int"));
    const auto str1 = toString(typedefDecl1);

    const auto typedefDecl2 = createTypedefDecl("SomeType", createTypeRef("someOtherType"), {Modifier::PRIVATE});
    const auto str2 = toString(typedefDecl2);

    const auto typedefDecl3 = createTypedefDecl("SomeTypeAgain", createTypeRef("double_t20"), {Modifier::PUBLIC, Modifier::OPEN, Modifier::STATIC});
    const auto str3 = toString(typedefDecl3);

    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(str1 + str2), KahwaFileBuilder().with({typedefDecl1, typedefDecl2}).build());
    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(str1 + str3), KahwaFileBuilder().with({typedefDecl1, typedefDecl3}).build());
    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(str2 + str3), KahwaFileBuilder().with({typedefDecl2, typedefDecl3}).build());
    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(str1 + str2 + str3), KahwaFileBuilder().with({typedefDecl1, typedefDecl2, typedefDecl3}).build());

    expectNoDiagnostics();
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
    .with(createTypeRef("superClass"))
    .build();
    const auto str1 = toString(classDecl1);

    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(str1), KahwaFileBuilder().with(classDecl1).build());

    const auto classDecl2 = ClassDeclBuilder("className")
    .with({createTypeRef("superClass1"), createTypeRef("superClass2")})
    .build();
    const auto str2 = toString(classDecl2);

    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(str2), KahwaFileBuilder().with(classDecl2).build());

    const auto classDecl3 = ClassDeclBuilder("className")
    .with({createTypeRef("superClass1"), createTypeRef("superClass2")})
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
        .with({createTypeRef("superClass1"), createTypeRef("superClass2")})
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

    const auto typedefDecl1 = createTypedefDecl("someTypeDef", TypeRefBuilder("int").build());
    const auto str3 = toString(typedefDecl1);

    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(str1 + str2 + str3),
        KahwaFileBuilder()
        .with({classDecl1, classDecl2})
        .with(typedefDecl1)
        .build());

    const auto typedefDecl2 = createTypedefDecl("anotherTypeDef", TypeRefBuilder("float").build());
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
