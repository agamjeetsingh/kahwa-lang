//
// Created by Agamjeet Singh on 28/12/25.
//

#ifndef STMTTESTFIXTURE_H
#define STMTTESTFIXTURE_H
#include "ExprTestFixture.h"


class StmtTestFixture : public ExprTestFixture {
protected:
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

    inline static std::vector<Stmt*> exampleStmts = {
        exprStmt(integerLiteral(1)), // 1;
        exprStmt(binaryExpr( // x++ + y;
            unaryExpr(identifierRef("x"), UnaryOp::POST_INCREMENT),
            identifierRef("y"),
            BinaryOp::PLUS)),
        exprStmt(binaryExpr(
            callExpr(identifierRef("a"), {identifierRef("x")}),
            callExpr(identifierRef("b"), {identifierRef("y")}),
            BinaryOp::PLUS)), // a(x) + b(y);
        returnStmt(stringLiteral("Hello, World!")),
        continueStmt()
    };

    static std::vector<Block*> getExampleBlocks() {
        std::vector<Block*> blocks;

        for (int i = 0; i < exampleStmts.size(); i++) {
            auto blockBuilder = BlockBuilder();
            for (int j = 0; j <= i; j++) {
                blockBuilder.with(exampleStmts[j]);
            }
            blocks.push_back(blockBuilder.build());
        }

        return blocks;
    }

};



#endif //STMTTESTFIXTURE_H
