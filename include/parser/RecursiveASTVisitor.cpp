//
// Created by Agamjeet Singh on 02/01/26.
//

#include "RecursiveASTVisitor.h"
#include "KahwaFile.h"
#include "ClassDecl.h"
#include "TypedefDecl.h"
#include "ModifierNode.h"
#include "Block.h"
#include "FieldDecl.h"
#include "MethodDecl.h"
#include "TypeParameterDecl.h"
#include "TypeRef.h"
#include "stmt/BreakStmt.h"
#include "stmt/ContinueStmt.h"
#include "stmt/ExprStmt.h"
#include "stmt/ForLoop.h"
#include "stmt/IfStmt.h"
#include "stmt/ReturnStmt.h"
#include "stmt/WhileLoop.h"
#include "expr/BinaryExpr.h"
#include "expr/CallExpr.h"
#include "expr/IdentifierRef.h"
#include "expr/IndexExpr.h"
#include "expr/MemberAccessExpr.h"
#include "expr/TernaryExpr.h"
#include "expr/UnaryExpr.h"
#include "expr/literals/BoolLiteral.h"
#include "expr/literals/FloatLiteral.h"
#include "expr/literals/IntegerLiteral.h"
#include "expr/literals/NullLiteral.h"
#include "expr/literals/StringLiteral.h"

void RecursiveASTVisitor::visit(KahwaFile* node) {
    node->visitChildren(*this);
}

void RecursiveASTVisitor::visit(ClassDecl* node) {
    node->visitChildren(*this);
}

void RecursiveASTVisitor::visit(TypedefDecl* node) {
    node->visitChildren(*this);
}

void RecursiveASTVisitor::visit(ModifierNode* node) {
    node->visitChildren(*this);
}

void RecursiveASTVisitor::visit(Block* node) {
    node->visitChildren(*this);
}

void RecursiveASTVisitor::visit(FieldDecl* node) {
    node->visitChildren(*this);
}

void RecursiveASTVisitor::visit(MethodDecl* node) {
    node->visitChildren(*this);
}

void RecursiveASTVisitor::visit(TypeParameterDecl* node) {
    node->visitChildren(*this);
}

void RecursiveASTVisitor::visit(TypeRef* node) {
    node->visitChildren(*this);
}

void RecursiveASTVisitor::visit(BreakStmt* node) {
    node->visitChildren(*this);
}

void RecursiveASTVisitor::visit(ContinueStmt* node) {
    node->visitChildren(*this);
}

void RecursiveASTVisitor::visit(ExprStmt* node) {
    node->visitChildren(*this);
}

void RecursiveASTVisitor::visit(ForLoop* node) {
    node->visitChildren(*this);
}

void RecursiveASTVisitor::visit(IfStmt* node) {
    node->visitChildren(*this);
}

void RecursiveASTVisitor::visit(ReturnStmt* node) {
    node->visitChildren(*this);
}

void RecursiveASTVisitor::visit(WhileLoop* node) {
    node->visitChildren(*this);
}

void RecursiveASTVisitor::visit(BinaryExpr* node) {
    node->visitChildren(*this);
}

void RecursiveASTVisitor::visit(CallExpr* node) {
    node->visitChildren(*this);
}

void RecursiveASTVisitor::visit(IdentifierRef* node) {
    node->visitChildren(*this);
}

void RecursiveASTVisitor::visit(IndexExpr* node) {
    node->visitChildren(*this);
}

void RecursiveASTVisitor::visit(MemberAccessExpr* node) {
    node->visitChildren(*this);
}

void RecursiveASTVisitor::visit(TernaryExpr* node) {
    node->visitChildren(*this);
}

void RecursiveASTVisitor::visit(UnaryExpr* node) {
    node->visitChildren(*this);
}

void RecursiveASTVisitor::visit(BoolLiteral* node) {
    node->visitChildren(*this);
}

void RecursiveASTVisitor::visit(FloatLiteral* node) {
    node->visitChildren(*this);
}

void RecursiveASTVisitor::visit(IntegerLiteral* node) {
    node->visitChildren(*this);
}

void RecursiveASTVisitor::visit(NullLiteral* node) {
    node->visitChildren(*this);
}

void RecursiveASTVisitor::visit(StringLiteral* node) {
    node->visitChildren(*this);
}
