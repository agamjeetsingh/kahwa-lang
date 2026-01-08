//
// Created by Agamjeet Singh on 28/12/25.
//

#ifndef STMTTESTFIXTURE_H
#define STMTTESTFIXTURE_H
#include "../../include/parser/FieldDecl.h"
#include "../../include/parser/stmt/VariableDecl.h"
#include "ExprTestFixture.h"


class StmtTestFixture : public ExprTestFixture {
protected:
    void testStmts(const std::vector<std::pair<std::string, Stmt*>>& tests) const {
        for (auto& [str, expectedStmt]: tests) {
            auto parsedStmt = parseStmt(str);
            EXPECT_PRED2(stmtEqualIgnoreSourceRange, parsedStmt, expectedStmt);
            std::cout << "parsedStmt: " << toString(parsedStmt) << "\nexpectedStmt: " << toString(expectedStmt) << "\n--------" << std::endl;
        }
    }

    void testStmts(const std::vector<Stmt*>& stmts) const {
        for (auto* expectedStmt: stmts) {
            auto parsedStmt = parseStmt(toString(expectedStmt));
            EXPECT_PRED2(stmtEqualIgnoreSourceRange, parsedStmt, expectedStmt);
            std::cout << "parsedStmt: " << toString(parsedStmt) << "\nexpectedStmt: " << toString(expectedStmt) << "\n--------" << std::endl;
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

    static IfStmt* ifStmt(Expr* cond, Block* ifBlock, Block* elseBlock = block({})) {
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

    static VariableDecl* variableDecl(const std::string& name, TypeRef* type, Expr* initExpr = nullptr, const std::vector<Modifier>& modifiers = {}) {
        std::vector<ModifierNode*> modifierNodes;
        std::ranges::transform(modifiers, std::back_inserter(modifierNodes), [](const Modifier& modifier) { return astArena.make<ModifierNode>(modifier, dummy_source); });
        return astArena.make<VariableDecl>(astArena.make<FieldDecl>(name, modifierNodes, type, dummy_source, dummy_source, dummy_source, initExpr));
    }

    static const std::vector<Expr*>& getExampleExprs() {
        static std::vector<Expr*> exampleExprs = {
            integerLiteral(1),
            binaryExpr( // x++ + y;
                unaryExpr(identifierRef("x"), UnaryOp::POST_INCREMENT),
                identifierRef("y"),
                BinaryOp::PLUS),
            binaryExpr(
                callExpr(identifierRef("a"), {identifierRef("x")}),
                callExpr(identifierRef("b"), {identifierRef("y")}),
                BinaryOp::PLUS),
            stringLiteral("Hello, World!")
        };
        return exampleExprs;
    }

    static const std::vector<Stmt*>& getExampleStmts() {
        static std::vector<Stmt*> exampleStmts = []() {
            std::vector<Stmt*> stmts;
            stmts.push_back(continueStmt());
            stmts.push_back(breakStmt());
            for (auto expr: getExampleExprs()) {
                stmts.push_back(returnStmt(expr));
                stmts.push_back(exprStmt(expr));
                stmts.push_back(ifStmt(expr, block({})));
                stmts.push_back(whileLoop(expr, block({})));
            }
            return stmts;
        }();
        return exampleStmts;
    }

    static const std::vector<Block*>& getExampleBlocks() {
        static std::vector<Block*> exampleBlocks = []() {
            std::vector<Block*> blocks;
            blocks.push_back(block({})); // {}
            const auto& stmts = getExampleStmts();
            for (int i = 0; i < stmts.size(); i++) {
                auto blockBuilder = BlockBuilder();
                for (int j = 0; j <= i; j++) {
                    blockBuilder.with(stmts[j]);
                }
                blocks.push_back(blockBuilder.build());

                blocks.push_back(block({block({stmts[i]})})); // { { stmts[i] } }
            }
            return blocks;
        }();
        return exampleBlocks;
    }

    const std::vector<Expr*>& exampleExprs = getExampleExprs();
    const std::vector<Stmt*>& exampleStmts = getExampleStmts();
    const std::vector<Block*>& exampleBlocks = getExampleBlocks();
};



#endif //STMTTESTFIXTURE_H
