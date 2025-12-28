//
// Created by Agamjeet Singh on 04/12/25.
//

#include <gtest/gtest.h>

#include "../DiagnosticTesting.h"
#include "../../include/diagnostics/DiagnosticEngine.h"
#include "../../include/parser/Parser.h"
#include "../../include/tokeniser/Tokeniser.h"
#include "../../include/parser/Modifier.h"
#include "../../include/parser/ASTBuilder.h"
#include "../../include/parser/expr/BinaryExpr.h"
#include "../../include/parser/expr/BinaryOp.h"
#include "../../include/parser/expr/CallExpr.h"
#include "../../include/parser/expr/IdentifierRef.h"
#include "../../include/parser/expr/MemberAccessExpr.h"
#include "../../include/parser/expr/TernaryExpr.h"
#include "../../include/parser/expr/UnaryExpr.h"
#include "../../include/parser/expr/UnaryOp.h"
#include "../../include/parser/expr/literals/BoolLiteral.h"
#include "../../include/parser/expr/literals/FloatLiteral.h"
#include "../../include/parser/expr/literals/IntegerLiteral.h"
#include "../../include/parser/expr/literals/NullLiteral.h"
#include "../../include/parser/expr/literals/StringLiteral.h"
#include "../../include/parser/stmt/BreakStmt.h"
#include "../../include/parser/stmt/ContinueStmt.h"
#include "../../include/parser/stmt/ExprStmt.h"
#include "../../include/parser/stmt/IfStmt.h"
#include "../../include/parser/stmt/ForLoop.h"
#include "../../include/parser/stmt/ReturnStmt.h"
#include "../../include/parser/stmt/WhileLoop.h"

class ParserTest : public testing::Test, public DiagnosticTesting {
protected:
    inline static Arena astArena;
    Parser parser{astArena, diagnostic_engine};
    Tokeniser tokeniser{diagnostic_engine};

    static inline SourceRange dummy_source = {0, 0};


    [[nodiscard]] KahwaFile* parseFile(const std::string &str) const {
        return parser.parseFile(tokeniser.tokenise(0, str));
    }

    [[nodiscard]] TypeRef* parseTypeRef(const std::string &str) const {
        return parser.parseTypeRef(tokeniser.tokenise(0, str));
    }

    [[nodiscard]] Expr* parseExpr(const std::string &str) const {
        return parser.parseExpr(tokeniser.tokenise(0, str));
    }

    [[nodiscard]] Stmt* parseStmt(const std::string &str) const {
        return parser.parseStmt(tokeniser.tokenise(0, str));
    }

    void testExprs(const std::vector<std::pair<std::string, Expr*>>& tests) const {
        for (auto& [str, expectedExpr]: tests) {
            auto parsedExpr = parseExpr(str);
            EXPECT_PRED2(exprEqualIgnoreSourceRange, parsedExpr, expectedExpr);
            std::cout << "parsedExpr: " << toString(parsedExpr) << "\nexpectedExpr: " << toString(expectedExpr) << "\n--------" << std::endl;
        }
    }

    void testStmts(const std::vector<std::pair<std::string, Stmt*>>& tests) const {
        for (auto& [str, expectedStmt]: tests) {
            auto parsedStmt = parseStmt(str);
            EXPECT_PRED2(stmtEqualIgnoreSourceRange, parsedStmt, expectedStmt);
            std::cout << "parsedExpr: " << toString(parsedStmt) << "\nexpectedExpr: " << toString(expectedStmt) << "\n--------" << std::endl;
        }
    }

    void testStmts(const std::vector<Stmt*>& stmts) const {
        for (auto* expectedStmt: stmts) {
            auto parsedStmt = parseStmt(toString(expectedStmt));
            EXPECT_PRED2(stmtEqualIgnoreSourceRange, parsedStmt, expectedStmt);
            std::cout << "parsedExpr: " << toString(parsedStmt) << "\nexpectedExpr: " << toString(expectedStmt) << "\n--------" << std::endl;
        }
    }

    static bool declEqualIgnoreSourceRange(const Decl* d1, const Decl* d2) {
        if (d1 == nullptr && d2 == nullptr) return true;
        if (d1 == nullptr || d2 == nullptr) return false;
        return d1->name == d2->name && d1->modifiers == d2->modifiers;
    }

    static bool typedefDeclEqualIgnoreSourceRange(const TypedefDecl* td1, const TypedefDecl* td2) {
        if (td1 == nullptr && td2 == nullptr) return true;
        if (td1 == nullptr || td2 == nullptr) return false;
        if (!declEqualIgnoreSourceRange(td1, td2)) return false;

        return typeRefEqualIgnoreSourceRange(td1->referredType, td2->referredType);
    }

    static bool typeRefEqualIgnoreSourceRange(const TypeRef* t1, const TypeRef* t2) {
        if (t1 == nullptr && t2 == nullptr) return true;
        if (t1 == nullptr || t2 == nullptr) return false;

        if (t1->args.size() != t2->args.size()) return false;

        for (int i = 0; i < t1->args.size(); i++) {
            if (!typeRefEqualIgnoreSourceRange(t1->args[i], t2->args[i])) return false;
        }

        return true;
    }

    static bool fieldDeclEqualIgnoreSourceRange(const FieldDecl* fd1, const FieldDecl* fd2) {
        if (fd1 == nullptr && fd2 == nullptr) return true;
        if (fd1 == nullptr || fd2 == nullptr) return false;
        if (!declEqualIgnoreSourceRange(fd1, fd2)) return false;
        
        if (fd1->typeRef == nullptr && fd2->typeRef == nullptr) return true;
        if (fd1->typeRef == nullptr || fd2->typeRef == nullptr) return false;
        return *fd1->typeRef == *fd2->typeRef;
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
            cd1->nestedClasses.size() != cd2->nestedClasses.size() ||
            cd1->typeParameters.size() != cd2->typeParameters.size()) {
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

        for (size_t i = 0; i < cd1->typeParameters.size(); ++i) {
            if (*cd1->typeParameters[i].first != *cd2->typeParameters[i].first ||
                cd1->typeParameters[i].second != cd2->typeParameters[i].second) return false;
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

    static bool exprEqualIgnoreSourceRange(const Expr* e1, const Expr* e2) {
        if (e1 == nullptr && e2 == nullptr) return true;
        if (e1 == nullptr || e2 == nullptr) return false;
        if (e1->kind != e2->kind) return false;

        switch (e1->kind) {
            case ExprKind::BOOL_LITERAL:
                return dynamic_cast<const BoolLiteral*>(e1)->val == dynamic_cast<const BoolLiteral*>(e2)->val;
            case ExprKind::FLOAT_LITERAL:
                return dynamic_cast<const FloatLiteral*>(e1)->val == dynamic_cast<const FloatLiteral*>(e2)->val;
            case ExprKind::INTEGER_LITERAL:
                return dynamic_cast<const IntegerLiteral*>(e1)->val == dynamic_cast<const IntegerLiteral*>(e2)->val;
            case ExprKind::NULL_LITERAL:
                return true;
            case ExprKind::STRING_LITERAL:
                return dynamic_cast<const StringLiteral*>(e1)->val == dynamic_cast<const StringLiteral*>(e2)->val;
            case ExprKind::BINARY_EXPR: {
                auto be1 = dynamic_cast<const BinaryExpr*>(e1);
                auto be2 = dynamic_cast<const BinaryExpr*>(e2);
                return be1->op == be2->op && exprEqualIgnoreSourceRange(be1->expr1, be2->expr1) && exprEqualIgnoreSourceRange(be1->expr2, be2->expr2);
            }
            case ExprKind::CALL_EXPR: {
                auto ce1 = dynamic_cast<const CallExpr*>(e1);
                auto ce2 = dynamic_cast<const CallExpr*>(e2);
                if (!exprEqualIgnoreSourceRange(ce1->callee, ce2->callee) || ce1->args.size() != ce2->args.size()) return false;

                for (int i = 0; i < ce1->args.size(); i++) {
                    if (!exprEqualIgnoreSourceRange(ce1->args[i], ce2->args[i])) return false;
                }
                return true;
            }
            case ExprKind::IDENTIFIER_REF: {
                return dynamic_cast<const IdentifierRef*>(e1)->name == dynamic_cast<const IdentifierRef*>(e2)->name;
            }
            case ExprKind::MEMBER_ACCESS_EXPR: {
                auto me1 = dynamic_cast<const MemberAccessExpr*>(e1);
                auto me2 = dynamic_cast<const MemberAccessExpr*>(e2);

                return me1->member == me2->member && exprEqualIgnoreSourceRange(me1->base, me2->base);
            }
            case ExprKind::TERNARY_EXPR: {
                auto te1 = dynamic_cast<const TernaryExpr*>(e1);
                auto te2 = dynamic_cast<const TernaryExpr*>(e2);

                return exprEqualIgnoreSourceRange(te1->cond, te2->cond) &&
                    exprEqualIgnoreSourceRange(te1->expr1, te2->expr1) &&
                        exprEqualIgnoreSourceRange(te1->expr2, te2->expr2);
            }
            case ExprKind::UNARY_EXPR: {
                auto ue1 = dynamic_cast<const UnaryExpr*>(e1);
                auto ue2 = dynamic_cast<const UnaryExpr*>(e2);

                return ue1->op == ue2->op && exprEqualIgnoreSourceRange(ue1->expr, ue2->expr);
            }
            case ExprKind::EXPR:
                return true;
        }
    }

    static bool stmtEqualIgnoreSourceRange(const Stmt* s1, const Stmt* s2) {
        if (s1 == nullptr && s2 == nullptr) return true;
        if (s1 == nullptr || s2 == nullptr) return false;
        if (s1->kind != s2->kind) return false;

        switch (s1->kind) {
            case StmtKind::BREAK:
            case StmtKind::CONTINUE:
                return true;
            case StmtKind::EXPR:
                return exprEqualIgnoreSourceRange(dynamic_cast<const ExprStmt*>(s1)->expr, dynamic_cast<const ExprStmt*>(s2)->expr);
            case StmtKind::FOR: {
                auto fs1 = dynamic_cast<const ForLoop*>(s1);
                auto fs2 = dynamic_cast<const ForLoop*>(s2);

                return stmtEqualIgnoreSourceRange(fs1->init, fs2->init) &&
                    exprEqualIgnoreSourceRange(fs1->cond, fs2->cond) &&
                        stmtEqualIgnoreSourceRange(fs1->step, fs2->step) &&
                            blockEqualIgnoreSourceRange(fs1->body, fs2->body);
            }
            case StmtKind::IF: {
                auto is1 = dynamic_cast<const IfStmt*>(s1);
                auto is2 = dynamic_cast<const IfStmt*>(s2);

                return exprEqualIgnoreSourceRange(is1->cond, is2->cond) &&
                    blockEqualIgnoreSourceRange(is1->ifBlock, is2->ifBlock) &&
                        blockEqualIgnoreSourceRange(is1->elseBlock, is2->elseBlock);
            }
            case StmtKind::RETURN:
                return exprEqualIgnoreSourceRange(dynamic_cast<const ReturnStmt*>(s1)->expr, dynamic_cast<const ReturnStmt*>(s2)->expr);
            case StmtKind::WHILE: {
                auto ws1 = dynamic_cast<const WhileLoop*>(s1);
                auto ws2 = dynamic_cast<const WhileLoop*>(s2);

                return exprEqualIgnoreSourceRange(ws1->cond, ws2->cond) && blockEqualIgnoreSourceRange(ws1->body, ws2->body);
            }
            case StmtKind::STMT:
                return true;
            case StmtKind::BLOCK:
                return blockEqualIgnoreSourceRange(dynamic_cast<const Block*>(s1), dynamic_cast<const Block*>(s2));
            case StmtKind::VARIABLE_DECL: {
                auto vs1 = dynamic_cast<const FieldDecl*>(s1);
                auto vs2 = dynamic_cast<const FieldDecl*>(s2);

                return typeRefEqualIgnoreSourceRange(vs1->typeRef, vs2->typeRef) &&
                    vs1->name == vs2->name &&
                        exprEqualIgnoreSourceRange(vs1->initExpr, vs2->initExpr) &&
                        modifiersEqualIgnoreSourceRange(vs1->modifiers, vs2->modifiers);
            }
        }
    }

    static bool modifiersEqualIgnoreSourceRange(const std::vector<ModifierNode>& nodes1, const std::vector<ModifierNode>& nodes2) {
        if (nodes1.size() != nodes2.size()) return false;

        for (int i = 0; i < nodes1.size(); i++) {
            if (nodes1[i].modifier != nodes2[i].modifier) return false;
        }

        return true;
    }

    static bool blockEqualIgnoreSourceRange(const Block* b1, const Block* b2) {
        if (b1 == nullptr && b2 == nullptr) return true;
        if (b1 == nullptr || b2 == nullptr) return false;
        if (b1->stmts.size() != b2->stmts.size()) return false;

        for (int i = 0; i < b1->stmts.size(); i++) {
            if (!stmtEqualIgnoreSourceRange(b1->stmts[i], b2->stmts[i])) return false;
        }

        return true;
    }

    static BoolLiteral* boolLiteral(bool val) {
        return astArena.make<BoolLiteral>(val, dummy_source);
    }

    static FloatLiteral* floatLiteral(float val) {
        return astArena.make<FloatLiteral>(val, dummy_source);
    }

    static IntegerLiteral* integerLiteral(int val) {
        return astArena.make<IntegerLiteral>(val, dummy_source);
    }

    static NullLiteral* nullLiteral() {
        return astArena.make<NullLiteral>(dummy_source);
    }

    static StringLiteral* stringLiteral(const std::string& val) {
        return astArena.make<StringLiteral>(val, dummy_source);
    }

    static BinaryExpr* binaryExpr(Expr* expr1, Expr* expr2, BinaryOp op) {
        return astArena.make<BinaryExpr>(expr1, expr2, op, dummy_source);
    }

    static CallExpr* callExpr(Expr* callee, const std::vector<Expr*>& args) {
        return astArena.make<CallExpr>(callee, args, dummy_source);
    }

    static IdentifierRef* identifierRef(const std::string& name) {
        return astArena.make<IdentifierRef>(name, dummy_source);
    }

    static MemberAccessExpr* memberAccessExpr(Expr* base, const std::string& member) {
        return astArena.make<MemberAccessExpr>(base, member, dummy_source);
    }

    static TernaryExpr* ternaryExpr(Expr* cond, Expr* expr1, Expr* expr2) {
        return astArena.make<TernaryExpr>(cond, expr1, expr2, dummy_source);
    }

    static UnaryExpr* unaryExpr(Expr* expr, UnaryOp op) {
        return astArena.make<UnaryExpr>(expr, op, dummy_source);
    }

    static BreakStmt* breakStmt() {
        return astArena.make<BreakStmt>(dummy_source);
    }

    static ContinueStmt* continueStmt() {
        return astArena.make<ContinueStmt>(dummy_source);
    }

    static ExprStmt* exprStmt(Expr* expr) {
        return astArena.make<ExprStmt>(expr, dummy_source);
    }

    static ForLoop* forLoop(Stmt* init, Expr* cond, Stmt* step, Block* body) {
        return astArena.make<ForLoop>(init, cond, step, body, dummy_source, dummy_source);
    }

    static IfStmt* ifStmt(Expr* cond, Block* ifBlock, Block* elseBlock = nullptr) {
        return astArena.make<IfStmt>(cond, ifBlock, elseBlock, dummy_source, dummy_source);
    }

    static ReturnStmt* returnStmt(Expr* expr) {
        return astArena.make<ReturnStmt>(expr, dummy_source, dummy_source);
    }

    static WhileLoop* whileLoop(Expr* cond, Block* block) {
        return astArena.make<WhileLoop>(cond, block, dummy_source, dummy_source);
    }

    static Block* block(const std::vector<Stmt*>& stmts) {
        return astArena.make<Block>(stmts, dummy_source);
    }

    static std::string toString(const std::vector<ModifierNode>& modifiers) {
        std::string str;
        for (int i = 0; i < modifiers.size(); i++) {
            str += ::toString(modifiers[i].modifier);
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

        if (!classDecl->typeParameters.empty()) {
            str += "<";

            for (int i = 0; i < classDecl->typeParameters.size(); i++) {
                if (classDecl->typeParameters[i].second == Variance::COVARIANT) {
                    str += "out ";
                } else if (classDecl->typeParameters[i].second == Variance::CONTRAVARIANT) {
                    str += "in ";
                }
                str += classDecl->typeParameters[i].first->toString();
                if (i != classDecl->typeParameters.size() - 1) {
                    str += ", ";
                }
            }

            str += ">";
        }

        if (!classDecl->superClasses.empty()) {
            str += " : ";

            for (int i = 0; i < classDecl->superClasses.size(); i++) {
                str += toString(classDecl->superClasses[i]);
                if (i != classDecl->superClasses.size() - 1) {
                    str += ", ";
                }
            }
        }

        str += " {\n";

        if (!classDecl->methods.empty()) {
            for (auto method : classDecl->methods) {
                str += toString(method);
                str += "\n";
            }
        }

        if (!classDecl->nestedClasses.empty()) {
            for (auto nestedClass: classDecl->nestedClasses) {
                str += toString(nestedClass);
                str += "\n";
            }
        }


        str += "}";

        return str;
    }

    static std::string toString(const MethodDecl* methodDecl) {
        std::string str = toString(methodDecl->modifiers);
        if (!methodDecl->modifiers.empty()) str += " ";

        str += toString(methodDecl->returnType);
        str += " ";
        str += methodDecl->name;
        str += "(";
        for (int i = 0; i < methodDecl->parameters.size(); i++) {
            str += toString(methodDecl->parameters[i].first);
            str += " ";
            str += methodDecl->parameters[i].second;
            if (i != methodDecl->parameters.size() - 1) str += ", ";
        }

        str += ") ";
        str += toString(methodDecl->block);
        return str;
    }

    static std::string toString(const Block* block) {
        std::string str = "{\n";
        for (auto stmt: block->stmts) {
            str += toString(stmt);
            str += "\n";
        }
        str += "}";
        return str;
    }

    static std::string toString(const Stmt* stmt) {
        if (stmt == nullptr) return "";
        switch (stmt->kind) {
            case StmtKind::BREAK:
                return "break;";
            case StmtKind::CONTINUE:
                return "continue;";
            case StmtKind::EXPR:
                return toString(dynamic_cast<const ExprStmt*>(stmt)->expr) + ";";
            case StmtKind::FOR: {
                auto fs = dynamic_cast<const ForLoop*>(stmt);
                return "for (" + toString(fs->init) + toString(fs->cond) + ";" + toString(fs->step) + ") " + toString(fs->body);
            }
            case StmtKind::IF: {
                auto is = dynamic_cast<const IfStmt*>(stmt);
                return "if (" + toString(is->cond) + ") " + toString(is->ifBlock) + (is->elseBlock ? " else " + toString(is->elseBlock) : "");
            }
            case StmtKind::RETURN:
                return "return " + toString(dynamic_cast<const ReturnStmt*>(stmt)->expr) + ";";
            case StmtKind::WHILE: {
                auto ws = dynamic_cast<const WhileLoop*>(stmt);
                return "while (" + toString(ws->cond) + ") " + toString(ws->body);
            }
            case StmtKind::STMT:
                return ";";
            case StmtKind::BLOCK:
                return toString(dynamic_cast<const Block*>(stmt));
            case StmtKind::VARIABLE_DECL: {
                auto vs = dynamic_cast<const FieldDecl*>(stmt);
                return toString(vs->typeRef) + " " + vs->name + (vs->initExpr ? " = " + toString(vs->initExpr) : "") + ";";
            }
        }
    }

    static std::string toString(const Expr* expr) {
        if (expr == nullptr) return "";
        switch (expr->kind) {
            case ExprKind::BOOL_LITERAL:
                return dynamic_cast<const BoolLiteral*>(expr)->val ? "true" : "false";
            case ExprKind::FLOAT_LITERAL:
                return std::to_string(dynamic_cast<const FloatLiteral*>(expr)->val);
            case ExprKind::INTEGER_LITERAL:
                return std::to_string(dynamic_cast<const IntegerLiteral*>(expr)->val);
            case ExprKind::NULL_LITERAL:
                return "null";
            case ExprKind::STRING_LITERAL:
                return dynamic_cast<const StringLiteral*>(expr)->val;
            case ExprKind::BINARY_EXPR: {
                auto be = dynamic_cast<const BinaryExpr*>(expr);
                return "(" + toString(be->expr1) + " " + ::toString(be->op) + " " + toString(be->expr2) + ")";
            }
            case ExprKind::CALL_EXPR: {
                auto ce = dynamic_cast<const CallExpr*>(expr);
                std::string str = "(" + toString(ce->callee) + "(";
                for (int i = 0; i < ce->args.size(); i++) {
                    str += toString(ce->args[i]);
                    if (i != ce->args.size() - 1) {
                        str += ", ";
                    }
                }
                str += "))";
                return str;
            }
            case ExprKind::IDENTIFIER_REF: {
                return dynamic_cast<const IdentifierRef*>(expr)->name;
            }
            case ExprKind::MEMBER_ACCESS_EXPR: {
                auto me = dynamic_cast<const MemberAccessExpr*>(expr);
                return "(" + toString(me->base) + "." + me->member + ")";
            }
            case ExprKind::TERNARY_EXPR: {
                auto te = dynamic_cast<const TernaryExpr*>(expr);
                return "(" + toString(te->cond) + " ? " + toString(te->expr1) + " : " + toString(te->expr2) + ")";
            }
            case ExprKind::UNARY_EXPR: {
                auto ue = dynamic_cast<const UnaryExpr*>(expr);
                if (ue->op == UnaryOp::POST_INCREMENT || ue->op == UnaryOp::POST_DECREMENT) {
                    return "("  + toString(ue->expr) + ::toString(ue->op) + ")";
                }
                return "(" + ::toString(ue->op) + toString(ue->expr) + ")";
            }
            case ExprKind::EXPR:
                return "dummy_expression";
        }
        return "unknown_expression";
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

TEST_F(ParserTest, ReportsCorrectDiagnosticWhenTypedefIsMalformedWithGenerics) {
    const auto typedefDecl = TypedefDeclBuilder(
        "pid",
        TypeRefBuilder("pair")
        .with(TypeRefBuilder("int").build())
        .with(TypeRefBuilder("double").build())
        .build()).build();

    const auto str = toString(typedefDecl);
    const std::vector<std::string> strs{
        "pair<", // identifier
        "pair<int", // greater
        "pair<int, ", // identifier
        "pair<int, double", // greater
        "pair<int, double,", // identifier
        "pair<,>", // identifier
        "pair<int double>", // greater, declaration
        "pair<int, double, >", // identifier
        "pair<pair<int, double>", // greater
        "pair<>", // not sure, TODO - Do something about it
        "pair<,,>"}; // identifier

    const std::vector<std::vector<DiagnosticKind>> expectedDiagnosticKinds{
        {DiagnosticKind::EXPECTED_IDENTIFIER},
        {DiagnosticKind::EXPECTED_GREATER},
        {DiagnosticKind::EXPECTED_IDENTIFIER},
        {DiagnosticKind::EXPECTED_GREATER},
        {DiagnosticKind::EXPECTED_IDENTIFIER},
        {DiagnosticKind::EXPECTED_IDENTIFIER},
        {DiagnosticKind::EXPECTED_GREATER, DiagnosticKind::EXPECTED_DECLARATION},
        {DiagnosticKind::EXPECTED_IDENTIFIER},
        {DiagnosticKind::EXPECTED_GREATER},
        {}, // TODO - Give error maybe
        {DiagnosticKind::EXPECTED_IDENTIFIER}
    };

    EXPECT_EQ(strs.size(), expectedDiagnosticKinds.size());

    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange, parseFile(str), KahwaFileBuilder().with(typedefDecl).build());

    expectNoDiagnostics();

    for (int i = 0; i < strs.size(); i++) {
        EXPECT_PRED2(typeRefEqualIgnoreSourceRange, parseTypeRef(strs[i]), nullptr);

        expectDiagnosticKindsIgnoreSourceRange(expectedDiagnosticKinds[i]);
    }
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

TEST_F(ParserTest, RecoversFromMalformedTypedefCorrectlyWithGenerics) {
    // TODO
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

TEST_F(ParserTest, ParsesMultipleClassesWithInheritanceAndTypeDefsCorrectly) {
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

TEST_F(ParserTest, ParsesClassesWithMethodsWithEmptyBodiesCorrectly) {
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

TEST_F(ParserTest, ParsesClassWithTypeParametersCorrectly) {
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

TEST_F(ParserTest, ParsesClassWithTypeParametersAndVarianceCorrectly) {
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

TEST_F(ParserTest, ParsesClassesWithFieldsCorrectly) {

}

TEST_F(ParserTest, ParsesTopLevelMethodsCorrectly) {
    const auto methodDecl1 = MethodDeclBuilder(
        "foo",
        TypeRefBuilder("float").build(),
        BlockBuilder().build()).build();
    const auto str1 = toString(methodDecl1);

    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange,
        parseFile(str1),
        KahwaFileBuilder().with(methodDecl1).build());

    const auto methodDecl2 = MethodDeclBuilder(
        "foo",
        TypeRefBuilder("int").build(),
        BlockBuilder().build())
        .with(Modifier::PRIVATE)
        .with(Modifier::STATIC)
        .with({TypeRefBuilder("int").build(), "length"})
        .with({TypeRefBuilder("vector") // vector<int>
            .with(TypeRefBuilder("int").build())
            .build(), "list"})
        .build();
    const auto str2 = toString(methodDecl2);

    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange,
        parseFile(str2),
        KahwaFileBuilder().with(methodDecl2).build());

    EXPECT_PRED2(kahwaFileEqualIgnoreSourceRange,
        parseFile(str1 + str2),
        KahwaFileBuilder().with({methodDecl1, methodDecl2}).build());

    expectNoDiagnostics();
}

TEST_F(ParserTest, ParsesLiteralsCorrectly) {
    std::vector<std::pair<std::string, Expr*>> strs = {
        {"123", integerLiteral(123)},
        {"true", boolLiteral(true)},
        {"false", boolLiteral(false)},
        {"3.14", floatLiteral(3.14)},
        {"null", nullLiteral()},
        {"\"\"", stringLiteral("")},
        {"\"a string!\"", stringLiteral("a string!")},
        {"abcdef_123", identifierRef("abcdef_123")}
    };

    testExprs(strs);

    expectNoDiagnostics();
}

TEST_F(ParserTest, ParsesBinaryExpressionsCorrectly) {
    std::vector<std::pair<std::string, Expr*>> strs1 = {
        {"1 + 2", binaryExpr(integerLiteral(1), integerLiteral(2), BinaryOp::PLUS)},
        {"1 + 2 + 3", binaryExpr(
            binaryExpr(integerLiteral(1), integerLiteral(2), BinaryOp::PLUS),
            integerLiteral(3),
            BinaryOp::PLUS)},
        {"1 + 2 * 3", binaryExpr(
            integerLiteral(1),
            binaryExpr(integerLiteral(2), integerLiteral(3), BinaryOp::STAR),
            BinaryOp::PLUS)},
        {"1 * 2 + 3", binaryExpr(
            binaryExpr(integerLiteral(1), integerLiteral(2), BinaryOp::STAR),
            integerLiteral(3),
            BinaryOp::PLUS)},
        {"1 - 2 - 3 - 4", binaryExpr(
            binaryExpr(
                binaryExpr(integerLiteral(1), integerLiteral(2), BinaryOp::MINUS),
                integerLiteral(3),
                BinaryOp::MINUS),
            integerLiteral(4),
            BinaryOp::MINUS)},
        {"1 + 2 - 3", binaryExpr(
            binaryExpr(integerLiteral(1), integerLiteral(2), BinaryOp::PLUS),
            integerLiteral(3),
            BinaryOp::MINUS)},
        {"true * abc + null", binaryExpr(
            binaryExpr(boolLiteral(true), identifierRef("abc"), BinaryOp::STAR),
            nullLiteral(),
            BinaryOp::PLUS)},
        {"a * b * c", binaryExpr(
            binaryExpr(identifierRef("a"), identifierRef("b"), BinaryOp::STAR),
            identifierRef("c"),
            BinaryOp::STAR)}
    };

    testExprs(strs1);

    expectNoDiagnostics();
}

TEST_F(ParserTest, ParsesExpressionsWithParenthesisCorrectly) {
    std::vector<std::pair<std::string, Expr*>> strs = {
        {"1 + (2 + 3)", binaryExpr(
            integerLiteral(1),
            binaryExpr(integerLiteral(2), integerLiteral(3), BinaryOp::PLUS),
            BinaryOp::PLUS)},
        {"(1 + 2) * 3", binaryExpr(
            binaryExpr(integerLiteral(1), integerLiteral(2), BinaryOp::PLUS),
            integerLiteral(3),
            BinaryOp::STAR)},
        {"1 + (2 + (3 + 4))", binaryExpr(
            integerLiteral(1),
            binaryExpr(
                integerLiteral(2),
                binaryExpr(integerLiteral(3), integerLiteral(4), BinaryOp::PLUS),
                BinaryOp::PLUS),
            BinaryOp::PLUS)},
        {"((1))", integerLiteral(1)},
        {"((1) + 2)", binaryExpr(integerLiteral(1), integerLiteral(2), BinaryOp::PLUS)},
        {"1 / (2 + 3)", binaryExpr(
            integerLiteral(1),
            binaryExpr(integerLiteral(2), integerLiteral(3), BinaryOp::PLUS),
            BinaryOp::SLASH)}
    };

    testExprs(strs);

    expectNoDiagnostics();
}

TEST_F(ParserTest, ParsesExpressionsWithUnaryOperatorsCorrectly) {
    std::vector<std::pair<std::string, Expr*>> strs = {
        {"+1", unaryExpr(integerLiteral(1), UnaryOp::PLUS)},
        {"-2", unaryExpr(integerLiteral(2), UnaryOp::MINUS)},
        {"!xyz", unaryExpr(identifierRef("xyz"), UnaryOp::NOT)},
        {"++x", unaryExpr(identifierRef("x"), UnaryOp::PRE_INCREMENT)},
        {"--x", unaryExpr(identifierRef("x"), UnaryOp::PRE_DECREMENT)},
        {"!(a + b)", unaryExpr(binaryExpr(
            identifierRef("a"),
            identifierRef("b"),
            BinaryOp::PLUS),
            UnaryOp::NOT)},
        {"!a + b", binaryExpr(
            unaryExpr(identifierRef("a"), UnaryOp::NOT),
            identifierRef("b"),
            BinaryOp::PLUS)},
        {"-(!a + b)", unaryExpr(parseExpr("!a + b"), UnaryOp::MINUS)},
        {"-1 + 2 + (-3)", binaryExpr(
        binaryExpr(unaryExpr(integerLiteral(1), UnaryOp::MINUS),
        integerLiteral(2),
        BinaryOp::PLUS),
        unaryExpr(integerLiteral(3), UnaryOp::MINUS),
        BinaryOp::PLUS)},
        {"++x - ++y * --z", binaryExpr(
            unaryExpr(identifierRef("x"), UnaryOp::PRE_INCREMENT), // ++x
            binaryExpr(
                unaryExpr(identifierRef("y"), UnaryOp::PRE_INCREMENT), // ++y
                unaryExpr(identifierRef("z"), UnaryOp::PRE_DECREMENT), // --z
                BinaryOp::STAR),
            BinaryOp::MINUS)},
        {"x++", unaryExpr(identifierRef("x"), UnaryOp::POST_INCREMENT)},
        {"y--", unaryExpr(identifierRef("y"), UnaryOp::POST_DECREMENT)},
        {"(x++ - !y) * --z", binaryExpr(
            binaryExpr(
            unaryExpr(identifierRef("x"), UnaryOp::POST_INCREMENT), // x++
                unaryExpr(identifierRef("y"), UnaryOp::NOT), // !y
                BinaryOp::MINUS),
            unaryExpr(identifierRef("z"), UnaryOp::PRE_DECREMENT), // --z
            BinaryOp::STAR)}
    };

    testExprs(strs);

    expectNoDiagnostics();
}

TEST_F(ParserTest, ParsesIndexingExpressionsCorrectly) {
    std::vector<std::pair<std::string, Expr*>> strs = {

    };

    testExprs(strs);

    expectNoDiagnostics();
}

TEST_F(ParserTest, ParsesCallExpressionCorrectly) {
    std::vector<std::pair<std::string, Expr*>> strs = {
        {"a()", callExpr(identifierRef("a"), {})},
        {"!a()", unaryExpr(callExpr(identifierRef("a"), {}), UnaryOp::NOT)},
        {"a(x)", callExpr(identifierRef("a"), {identifierRef("x")})},
        {"b(x, y++, 1, --z)", callExpr(identifierRef("b"), {
            identifierRef("x"),
            unaryExpr(identifierRef("y"), UnaryOp::POST_INCREMENT),
            integerLiteral(1),
            unaryExpr(identifierRef("z"), UnaryOp::PRE_DECREMENT)
        })},
        {"a(x) + b(y)", binaryExpr(
            callExpr(identifierRef("a"), {identifierRef("x")}),
            callExpr(identifierRef("b"), {identifierRef("y")}),
            BinaryOp::PLUS)},
        {"a(x, y) + b(x--, z)++", binaryExpr(
            callExpr(identifierRef("a"), {identifierRef("x"), identifierRef("y")}),
            unaryExpr(callExpr(identifierRef("b"), {unaryExpr(identifierRef("x"), UnaryOp::POST_DECREMENT), identifierRef("z")}), UnaryOp::POST_INCREMENT),
            BinaryOp::PLUS)}
    };

    testExprs(strs);

    expectNoDiagnostics();
}

// ===== Statements =====

TEST_F(ParserTest, ParsesOneWordStatementsCorrectly) {
    std::vector<Stmt*> stmts = {
        continueStmt(),
        breakStmt()
    };

    testStmts(stmts);

    expectNoDiagnostics();
}

TEST_F(ParserTest, ParsesReturnStatementCorrectly) {
    std::vector<Stmt*> stmts = {
        returnStmt(integerLiteral(1)), // return 1;
        returnStmt(binaryExpr( // return x++ + y;
            unaryExpr(identifierRef("x"), UnaryOp::POST_INCREMENT),
            identifierRef("y"),
            BinaryOp::PLUS)),
        returnStmt(binaryExpr(
            callExpr(identifierRef("a"), {identifierRef("x")}),
            callExpr(identifierRef("b"), {identifierRef("y")}),
            BinaryOp::PLUS)), // return a(x) + b(y);
        returnStmt(nullptr) // return;
    };

    testStmts(stmts);

    expectNoDiagnostics();
}

TEST_F(ParserTest, ParsesExpressionStatementsCorrectly) {
    std::vector<Stmt*> stmts = {
        exprStmt(integerLiteral(1)), // 1;
        exprStmt(binaryExpr( // x++ + y;
            unaryExpr(identifierRef("x"), UnaryOp::POST_INCREMENT),
            identifierRef("y"),
            BinaryOp::PLUS)),
        exprStmt(binaryExpr(
            callExpr(identifierRef("a"), {identifierRef("x")}),
            callExpr(identifierRef("b"), {identifierRef("y")}),
            BinaryOp::PLUS)), // a(x) + b(y);
        exprStmt(nullptr) // ;
    };

    testStmts(stmts);

    expectNoDiagnostics();
}

TEST_F(ParserTest, ParsesIfStmtCorrectly) {
    std::vector<Stmt*> stmts = {
        ifStmt(boolLiteral(true), block({continueStmt()})), // if (true) { continue; }
        ifStmt(unaryExpr(identifierRef("x"), UnaryOp::POST_INCREMENT), // if (x++) { ++x; break; continue; }
            block({
                exprStmt(unaryExpr(identifierRef("x"), UnaryOp::PRE_INCREMENT)),
                breakStmt(),
                continueStmt()}))
    };

    testStmts(stmts);

    expectNoDiagnostics();
}
