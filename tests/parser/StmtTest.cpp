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
    std::vector<Stmt*> stmts;
    stmts.reserve(exampleExprs.size());

    for (auto expr: exampleExprs) stmts.push_back(returnStmt(expr));

    stmts.push_back(returnStmt(nullptr)); // TODO - Not sure how to model empty expression for "return;"

    testStmts(stmts);

    expectNoDiagnostics();
}

TEST_F(StmtTestFixture, ParsesBlocksCorrectly) {
    std::vector<Stmt*> stmts;
    stmts.reserve(exampleBlocks.size());

    for (auto block: exampleBlocks) stmts.push_back(block);

    testStmts(stmts);

    expectNoDiagnostics();
}

TEST_F(StmtTestFixture, ParsesExpressionStatementsCorrectly) {
    std::vector<Stmt*> stmts;
    stmts.reserve(exampleExprs.size());

    for (auto expr: exampleExprs) stmts.push_back(exprStmt(expr));

    testStmts(stmts);

    expectNoDiagnostics();
}

TEST_F(StmtTestFixture, ParsesIfStmtCorrectly) {
    std::vector<Stmt*> stmts;

    for (auto cond: exampleExprs) {
        for (auto ifBlock: exampleBlocks) {
            stmts.push_back(ifStmt(cond, ifBlock));
        }

        for (auto ifBlock: exampleBlocks) {
            for (auto elseBlock: exampleBlocks) {
                stmts.push_back(ifStmt(cond, ifBlock, elseBlock));
            }
        }
    }

    testStmts(stmts);

    expectNoDiagnostics();
}

TEST_F(StmtTestFixture, ParsesWhileLoopCorrectly) {
    std::vector<Stmt*> stmts;

    for (auto cond: exampleExprs) {
        for (auto body: exampleBlocks) {
            stmts.push_back(whileLoop(cond, body));
        }
    }

    testStmts(stmts);

    expectNoDiagnostics();
}

TEST_F(StmtTestFixture, ParsesVariableDeclCorrectly) {
    std::vector<Stmt*> stmts;

    std::vector types = {
        TypeRefBuilder("int").build(),
        TypeRefBuilder("vector").with(TypeRefBuilder("int").build()).build()
    };

    for (auto type: types) {
        stmts.push_back(variableDecl("foo", type));
        stmts.push_back(variableDecl("foo", type, nullptr, {Modifier::STATIC, Modifier::STATIC}));
        for (auto expr: exampleExprs) {
            stmts.push_back(variableDecl("foo", type, expr));
            stmts.push_back(variableDecl("foo", type, expr, {Modifier::STATIC, Modifier::STATIC}));
        }
    }

    testStmts(stmts);

    expectNoDiagnostics();
}

TEST_F(StmtTestFixture, ParsesForLoopCorrectly) {
    // Fix: TODO - Extra colon after step statement
    std::vector<Stmt*> stmts;
    for (int i = 0; i < exampleStmts.size(); i++) {
        for (int j = 0; j < exampleExprs.size(); j++) {
            for (int k = 0; k < exampleStmts.size(); k++) {
                std::ranges::for_each(exampleBlocks, [&stmts, i, j, k, this](Block* block) {
                    stmts.push_back(forLoop(exampleStmts[i], exampleExprs[j], exampleStmts[k], block));
                });
            }
        }
    }

    testStmts(stmts);

    expectNoDiagnostics();
}