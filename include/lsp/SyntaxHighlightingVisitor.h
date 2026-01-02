//
// Created by Agamjeet Singh on 31/12/25.
//

#ifndef SYNTAXHIGHLIGHTINGVISITOR_H
#define SYNTAXHIGHLIGHTINGVISITOR_H
#include "LanguageServer.h"
#include "../parser/expr/BinaryExpr.h"
#include "../parser/expr/CallExpr.h"
#include "../parser/expr/IdentifierRef.h"
#include "../parser/expr/IndexExpr.h"
#include "../parser/expr/TernaryExpr.h"
#include "../parser/expr/UnaryExpr.h"
#include "../parser/expr/literals/BoolLiteral.h"
#include "../parser/expr/literals/FloatLiteral.h"
#include "../parser/expr/literals/IntegerLiteral.h"
#include "../parser/stmt/BreakStmt.h"
#include "../parser/stmt/ContinueStmt.h"
#include "../parser/stmt/ExprStmt.h"
#include "../parser/stmt/ForLoop.h"
#include "../parser/stmt/IfStmt.h"
#include "../parser/stmt/ReturnStmt.h"
#include "../parser/stmt/WhileLoop.h"
#include "../parser/expr/literals/NullLiteral.h"
#include "../parser/expr/literals/StringLiteral.h"


class SemanticAnalyser;

class SyntaxHighlightingVisitor : ASTVisitor {
public:
    SyntaxHighlightingVisitor(std::vector<tokenData>& data, SemanticAnalyser& semanticAnalyser):
    data(data), semanticAnalyser(semanticAnalyser) {}

    void visit(KahwaFile *node) override {
        node->visitChildren(*this);
    }

    void visit(ClassDecl *node) override {
        auto classSymbol = dynamic_cast<ClassSymbol*>(nodeToSymbol[node]);

        // **class** MyClass { ... }
        data.emplace_back(node->classSourceRange, LSPTokenType::KEYWORD, std::vector<LSPTokenModifier>{});

        std::vector nameLspTokenModifiers = {LSPTokenModifier::DECLARATION};
        if (classSymbol->isAbstract) nameLspTokenModifiers.push_back(LSPTokenModifier::ABSTRACT);

        // class **myClass** { ... }
        data.emplace_back(node->nameSourceRange, LSPTokenType::CLASS, nameLspTokenModifiers);
        // class myClass **{ ... }**
        node->visitChildren(*this);
    }

    void visit(TypedefDecl *node) override {

        // private **typedef** int myInt
        data.emplace_back(node->typedefSourceRange, LSPTokenType::KEYWORD, std::vector<LSPTokenModifier>{});
        // private typedef int **myInt**
        data.emplace_back(node->nameSourceRange, LSPTokenType::TYPE, std::vector{LSPTokenModifier::DECLARATION});
        // **private** typedef **int** myInt
        node->visitChildren(*this);
    }

    void visit(ModifierNode *node) override {
        data.emplace_back(node->bodyRange, LSPTokenType::MODIFIER, std::vector<LSPTokenModifier>{});
    }

    void visit(Block *node) override {
        node->visitChildren(*this);
    }

    void visit(FieldDecl *node) override {
        auto variableSymbol = dynamic_cast<VariableSymbol*>(nodeToSymbol[node]);
        std::vector nameModifiers = {LSPTokenModifier::DECLARATION};
        if (variableSymbol->isStatic) nameModifiers.push_back(LSPTokenModifier::STATIC);
    }

    void visit(MethodDecl *node) override {
        // void **foo**(...) { ... }
        auto functionSymbol = dynamic_cast<FunctionSymbol*>(nodeToSymbol[node]);
        std::vector nameLspTokenModifiers = {LSPTokenModifier::DECLARATION};
        if (functionSymbol->isStatic) nameLspTokenModifiers.push_back(LSPTokenModifier::STATIC);
        if (auto methodSymbol = dynamic_cast<MethodSymbol*>(functionSymbol); methodSymbol->isAbstract) nameLspTokenModifiers.push_back(LSPTokenModifier::ABSTRACT);
        auto tokenType = dynamic_cast<MethodSymbol*>(functionSymbol) ? LSPTokenType::METHOD : LSPTokenType::FUNCTION;
        data.emplace_back(node->nameSourceRange, tokenType, nameLspTokenModifiers);

        // **void** foo**(...)** **{ ... }**
        node->visitChildren(*this);
    }

    void visit(TypeRef *node) override {
        auto type = dynamic_cast<Type*>(nodeToSymbol[node]);

        // **pair**<int, T>
        if (auto classSymbol = dynamic_cast<ClassSymbol*>(type->typeSymbol)) {
            data.emplace_back(node->bodyRange, getClassTokenType(classSymbol), std::vector<LSPTokenModifier>{});
        } else {
            // type parameter like T
            // TODO - About it being a declaration, fix that by (HARD)
            data.emplace_back(node->bodyRange, LSPTokenType::TYPE_PARAMETER, std::vector<LSPTokenModifier>{});
        }

        // pair**<int, T>**
        node->visitChildren(*this);
    }

    void visit(BreakStmt *node) override {
        data.emplace_back(node->bodyRange, LSPTokenType::KEYWORD, std::vector<LSPTokenModifier>{});
    }

    void visit(ContinueStmt *node) override {
        data.emplace_back(node->bodyRange, LSPTokenType::KEYWORD, std::vector<LSPTokenModifier>{});
    }

    void visit(ExprStmt *node) override {
        // TODO - Not sure what to do (HARD)
        node->visitChildren(*this);
    }

    void visit(ForLoop *node) override {
        data.emplace_back(node->forRange, LSPTokenType::KEYWORD, std::vector<LSPTokenModifier>{});

        node->visitChildren(*this);
    }

    void visit(IfStmt *node) override {
        data.emplace_back(node->ifRange, LSPTokenType::KEYWORD, std::vector<LSPTokenModifier>{});

        node->visitChildren(*this);
    }

    void visit(ReturnStmt *node) override {
        data.emplace_back(node->returnRange, LSPTokenType::KEYWORD, std::vector<LSPTokenModifier>{});

        node->visitChildren(*this);
    }

    void visit(WhileLoop *node) override {
        data.emplace_back(node->whileRange, LSPTokenType::KEYWORD, std::vector<LSPTokenModifier>{});

        node->visitChildren(*this);
    }

    void visit(BinaryExpr *node) override {
        // TODO - Check if node->op is operator overloaded

        node->visitChildren(*this);
    }

    void visit(CallExpr *node) override {
        // TODO - Check for operator overload of ()

        node->visitChildren(*this);
    }

    void visit(IdentifierRef *node) override {
        // TODO - Could be field, variable, this, its HARD
    }

    void visit(IndexExpr *node) override {
        // TODO - Check for operator overload of []

        node->visitChildren(*this);
    }

    void visit(MemberAccessExpr *node) override {
        // TODO - HARD
    }

    void visit(TernaryExpr *node) override {
        node->visitChildren(*this);
    }

    void visit(UnaryExpr *node) override {
        // TODO - Check for operator overload of node->op

        node->visitChildren(*this);
    }

    void visit(BoolLiteral *node) override {
        data.emplace_back(node->bodyRange, LSPTokenType::KEYWORD, std::vector<LSPTokenModifier>{});
    }

    void visit(FloatLiteral *node) override {
        data.emplace_back(node->bodyRange, LSPTokenType::NUMBER, std::vector<LSPTokenModifier>{});
    }

    void visit(IntegerLiteral *node) override {
        data.emplace_back(node->bodyRange, LSPTokenType::NUMBER, std::vector<LSPTokenModifier>{});
    }

    void visit(NullLiteral *node) override {
        data.emplace_back(node->bodyRange, LSPTokenType::KEYWORD, std::vector<LSPTokenModifier>{});
    }

    void visit(StringLiteral *node) override {
        data.emplace_back(node->bodyRange, LSPTokenType::STRING, std::vector<LSPTokenModifier>{});
    }

private:
    std::vector<tokenData>& data;
    SemanticAnalyser& semanticAnalyser;
    std::unordered_map<ASTNode*, Symbol*> nodeToSymbol;

    static LSPTokenType getClassTokenType(ClassSymbol* classSymbol) {
        return classSymbol->isInterface ? LSPTokenType::CLASS : LSPTokenType::INTERFACE;
    }
};



#endif //SYNTAXHIGHLIGHTINGVISITOR_H
