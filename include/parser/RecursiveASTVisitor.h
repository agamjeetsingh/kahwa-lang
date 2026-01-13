//
// Created by Agamjeet Singh on 02/01/26.
//

#ifndef RECURSIVEASTVISITOR_H
#define RECURSIVEASTVISITOR_H

#include "ASTVisitor.h"

class RecursiveASTVisitor : public ASTVisitor {
public:
    void visit(KahwaFile* node) override;
    void visit(ClassDecl* node) override;
    void visit(TypedefDecl* node) override;
    void visit(ModifierNode* node) override;
    void visit(Block* node) override;
    void visit(FieldDecl* node) override;
    void visit(MethodDecl* node) override;
    void visit(TypeParameterDecl* node) override;
    void visit(TypeRef* node) override;
    void visit(VariableDecl *node) override;
    void visit(BreakStmt* node) override;
    void visit(ContinueStmt* node) override;
    void visit(ExprStmt* node) override;
    void visit(ForLoop* node) override;
    void visit(IfStmt* node) override;
    void visit(ReturnStmt* node) override;
    void visit(WhileLoop* node) override;
    void visit(BinaryExpr* node) override;
    void visit(CallExpr* node) override;
    void visit(IdentifierRef* node) override;
    void visit(IndexExpr* node) override;
    void visit(MemberAccessExpr* node) override;
    void visit(TernaryExpr* node) override;
    void visit(UnaryExpr* node) override;
    void visit(BoolLiteral* node) override;
    void visit(FloatLiteral* node) override;
    void visit(IntegerLiteral* node) override;
    void visit(NullLiteral* node) override;
    void visit(StringLiteral* node) override;
};

#endif //RECURSIVEASTVISITOR_H
