//
// Created by Agamjeet Singh on 02/01/26.
//

#include "../../include/parser/RecursiveASTVisitor.h"
#include "../../include/parser/KahwaFile.h"
#include "../../include/parser/ClassDecl.h"
#include "../../include/parser/TypedefDecl.h"
#include "../../include/parser/ModifierNode.h"
#include "../../include/parser/Block.h"
#include "../../include/parser/FieldDecl.h"
#include "../../include/parser/MethodDecl.h"
#include "../../include/parser/TypeParameterDecl.h"
#include "../../include/parser/TypeRef.h"
#include "../../include/parser/stmt/BreakStmt.h"
#include "../../include/parser/stmt/ContinueStmt.h"
#include "../../include/parser/stmt/ExprStmt.h"
#include "../../include/parser/stmt/ForLoop.h"
#include "../../include/parser/stmt/IfStmt.h"
#include "../../include/parser/stmt/ReturnStmt.h"
#include "../../include/parser/stmt/WhileLoop.h"
#include "../../include/parser/expr/BinaryExpr.h"
#include "../../include/parser/expr/CallExpr.h"
#include "../../include/parser/expr/IdentifierRef.h"
#include "../../include/parser/expr/IndexExpr.h"
#include "../../include/parser/expr/MemberAccessExpr.h"
#include "../../include/parser/expr/TernaryExpr.h"
#include "../../include/parser/expr/UnaryExpr.h"
#include "../../include/parser/expr/literals/BoolLiteral.h"
#include "../../include/parser/expr/literals/FloatLiteral.h"
#include "../../include/parser/expr/literals/IntegerLiteral.h"
#include "../../include/parser/expr/literals/NullLiteral.h"
#include "../../include/parser/expr/literals/StringLiteral.h"
#include "../../include/parser/stmt/VariableDecl.h"

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

void RecursiveASTVisitor::visit(VariableDecl *node) {
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
