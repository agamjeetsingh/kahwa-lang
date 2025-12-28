//
// Created by Agamjeet Singh on 28/12/25.
//

#include "StmtTestFixture.h"


TEST_F(StmtTestFixture, ParsesOneWordStatementsCorrectly) {
    std::vector<Stmt*> stmts = {
        continueStmt(),
        breakStmt()
    };

    testStmts(stmts);

    expectNoDiagnostics();
}

TEST_F(StmtTestFixture, ParsesReturnStatementCorrectly) {
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

TEST_F(StmtTestFixture, ParsesExpressionStatementsCorrectly) {
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

TEST_F(StmtTestFixture, ParsesIfStmtCorrectly) {
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

TEST_F(StmtTestFixture, ParsesWhileLoopCorrectly) {
    std::vector<Stmt*> stmts;
    std::ranges::for_each(getExampleBlocks(), [&stmts](Block* block) {
        stmts.push_back(whileLoop(boolLiteral(true), block));
    });

    testStmts(stmts);

    expectNoDiagnostics();
}

TEST_F(StmtTestFixture, ParsesForLoopCorrectly) {
    // Fix: TODO - Extra colon after step statement
    std::vector<Stmt*> stmts;
    for (int i = 0; i < exampleStmts.size(); i++) {
        for (int j = 0; j < exampleExprs.size(); j++) {
            for (int k = 0; k < exampleStmts.size(); k++) {
                std::ranges::for_each(getExampleBlocks(), [&stmts, i, j, k](Block* block) {
                    stmts.push_back(forLoop(exampleStmts[i], exampleExprs[j], exampleStmts[k], block));
                });
            }
        }
    }

    testStmts(stmts);

    expectNoDiagnostics();
}