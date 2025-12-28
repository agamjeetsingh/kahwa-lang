//
// Created by Agamjeet Singh on 28/12/25.
//

#ifndef EXPRTESTFIXTURE_H
#define EXPRTESTFIXTURE_H

#include "ParserTestBase.cpp"
#include "../../include/parser/expr/IndexExpr.h"

class ExprTestFixture : public ParserTestBase, public testing::Test {
    protected:
    void testExprs(const std::vector<std::pair<std::string, Expr*>>& tests) const {
        for (auto& [str, expectedExpr]: tests) {
            auto parsedExpr = parseExpr(str);
            EXPECT_PRED2(exprEqualIgnoreSourceRange, parsedExpr, expectedExpr);
            std::cout << "parsedExpr: " << toString(parsedExpr) << "\nexpectedExpr: " << toString(expectedExpr) << "\n--------" << std::endl;
        }
    }

    static BoolLiteral* boolLiteral(bool val) {
        return astArena.make<BoolLiteral>(val, dummy_source);
    }

    static FloatLiteral* floatLiteral(float val) {
        return astArena.make<FloatLiteral>(val, dummy_source);
    }

    static IntegerLiteral* integerLiteral(int val) {
        return astArena.make<IntegerLiteral>(val, dummy_source);
    }

    static NullLiteral* nullLiteral() {
        return astArena.make<NullLiteral>(dummy_source);
    }

    static StringLiteral* stringLiteral(const std::string& val) {
        return astArena.make<StringLiteral>(val, dummy_source);
    }

    static BinaryExpr* binaryExpr(Expr* expr1, Expr* expr2, BinaryOp op) {
        return astArena.make<BinaryExpr>(expr1, expr2, op, dummy_source);
    }

    static CallExpr* callExpr(Expr* callee, const std::vector<Expr*>& args) {
        return astArena.make<CallExpr>(callee, args, dummy_source);
    }

    static IdentifierRef* identifierRef(const std::string& name) {
        return astArena.make<IdentifierRef>(name, dummy_source);
    }

    static MemberAccessExpr* memberAccessExpr(Expr* base, const std::string& member) {
        return astArena.make<MemberAccessExpr>(base, member, dummy_source);
    }

    static TernaryExpr* ternaryExpr(Expr* cond, Expr* expr1, Expr* expr2) {
        return astArena.make<TernaryExpr>(cond, expr1, expr2, dummy_source);
    }

    static UnaryExpr* unaryExpr(Expr* expr, UnaryOp op) {
        return astArena.make<UnaryExpr>(expr, op, dummy_source);
    }

    static IndexExpr* indexExpr(Expr* callee, Expr* arg) {
        return astArena.make<IndexExpr>(callee, arg, dummy_source);
    }

    inline static std::vector<Expr*> exampleExprs = {
        integerLiteral(2), // 2
        binaryExpr( // x++ + y;
            unaryExpr(identifierRef("x"), UnaryOp::POST_INCREMENT),
            identifierRef("y"),
            BinaryOp::PLUS),
        binaryExpr(
        callExpr(identifierRef("a"), {identifierRef("x")}),
        callExpr(identifierRef("b"), {identifierRef("y")}),
        BinaryOp::PLUS) // a(x) + b(y)
    };
};



#endif //EXPRTESTFIXTURE_H
