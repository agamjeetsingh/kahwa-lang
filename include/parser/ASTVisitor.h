//
// Created by Agamjeet Singh on 31/12/25.
//

#ifndef ASTVISITOR_H
#define ASTVISITOR_H


struct VariableDecl;
struct TypeParameterDecl;
struct StringLiteral;
struct NullLiteral;
struct IntegerLiteral;
struct FloatLiteral;
struct BoolLiteral;
struct UnaryExpr;
struct TernaryExpr;
struct MemberAccessExpr;
struct IndexExpr;
struct IdentifierRef;
struct Expr;
struct CallExpr;
struct BinaryExpr;
struct WhileLoop;
struct ReturnStmt;
struct IfStmt;
struct ForLoop;
struct ExprStmt;
struct ContinueStmt;
struct BreakStmt;
struct KahwaFile;
struct ModifierNode;
struct ClassDecl;
struct TypedefDecl;
struct Block;
struct FieldDecl;
struct MethodDecl;
struct TypeRef;

struct ASTVisitor {
    virtual void visit(KahwaFile* node) = 0;
    virtual void visit(ClassDecl* node) = 0;
    virtual void visit(TypedefDecl* node) = 0;
    virtual void visit(ModifierNode* node) = 0;
    virtual void visit(Block* node) = 0;
    virtual void visit(FieldDecl* node) = 0;
    virtual void visit(MethodDecl* node) = 0;
    virtual void visit(TypeParameterDecl* node) = 0;
    virtual void visit(TypeRef* node) = 0;
    virtual void visit(BreakStmt* node) = 0;
    virtual void visit(ContinueStmt* node) = 0;
    virtual void visit(ExprStmt* node) = 0;
    virtual void visit(VariableDecl* node) = 0;
    virtual void visit(ForLoop* node) = 0;
    virtual void visit(IfStmt* node) = 0;
    virtual void visit(ReturnStmt* node) = 0;
    virtual void visit(WhileLoop* node) = 0;
    virtual void visit(BinaryExpr* node) = 0;
    virtual void visit(CallExpr* node) = 0;
    virtual void visit(IdentifierRef* node) = 0;
    virtual void visit(IndexExpr* node) = 0;
    virtual void visit(MemberAccessExpr* node) = 0;
    virtual void visit(TernaryExpr* node) = 0;
    virtual void visit(UnaryExpr* node) = 0;
    virtual void visit(BoolLiteral* node) = 0;
    virtual void visit(FloatLiteral* node) = 0;
    virtual void visit(IntegerLiteral* node) = 0;
    virtual void visit(NullLiteral* node) = 0;
    virtual void visit(StringLiteral* node) = 0;

    virtual ~ASTVisitor() = default;
};




#endif //ASTVISITOR_H
