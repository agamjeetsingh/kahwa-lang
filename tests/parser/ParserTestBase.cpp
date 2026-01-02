//
// Created by Agamjeet Singh on 28/12/25.
//

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
#include "../../include/parser/expr/IndexExpr.h"
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

class ParserTestBase : public DiagnosticTesting {
protected:
    inline static Arena astArena;
    Parser parser{astArena, diagnostic_engine};
    Tokeniser tokeniser{diagnostic_engine};

    static inline SourceRange dummy_source = {0, 0};

    // ===== Parser wrappers =====

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

    // ===== Equality functions =====

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
            if (cd1->typeParameters[i]->name != cd2->typeParameters[i]->name ||
                cd1->typeParameters[i]->variance != cd2->typeParameters[i]->variance) return false;
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
            case ExprKind::INDEX_EXPR: {
                auto ie1 = dynamic_cast<const IndexExpr*>(e1);
                auto ie2 = dynamic_cast<const IndexExpr*>(e2);
                return exprEqualIgnoreSourceRange(ie1->callee, ie2->callee) && exprEqualIgnoreSourceRange(ie1->arg, ie2->arg);
            }
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

    static bool modifiersEqualIgnoreSourceRange(const std::vector<ModifierNode*>& nodes1, const std::vector<ModifierNode*>& nodes2) {
        if (nodes1.size() != nodes2.size()) return false;

        for (int i = 0; i < nodes1.size(); i++) {
            if (nodes1[i]->modifier != nodes2[i]->modifier) return false;
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

    // ==== toString functions =====

    static std::string toString(const std::vector<ModifierNode*>& modifiers) {
        std::string str;
        for (int i = 0; i < modifiers.size(); i++) {
            str += ::toString(modifiers[i]->modifier);
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
                if (classDecl->typeParameters[i]->variance == Variance::COVARIANT) {
                    str += "out ";
                } else if (classDecl->typeParameters[i]->variance == Variance::CONTRAVARIANT) {
                    str += "in ";
                }
                str += classDecl->typeParameters[i]->name;
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
            case ExprKind::INDEX_EXPR: {
                auto ie = dynamic_cast<const IndexExpr*>(expr);
                return "(" + toString(ie->callee) + "[" + toString(ie->arg) + "])";
            }
        }
        return "unknown_expression";
    }


};