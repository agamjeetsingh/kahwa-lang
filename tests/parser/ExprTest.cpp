//
// Created by Agamjeet Singh on 28/12/25.
//

#include "ExprTestFixture.h"

TEST_F(ExprTestFixture, ParsesLiteralsCorrectly) {
    std::vector<std::pair<std::string, Expr*>> strs = {
        {"123", integerLiteral(123)},
        {"true", boolLiteral(true)},
        {"false", boolLiteral(false)},
        {"3.14", floatLiteral(3.14)},
        {"null", nullLiteral()},
        {"\"\"", stringLiteral("")},
        {"\"a string!\"", stringLiteral("a string!")},
        {"abcdef_123", identifierRef("abcdef_123")}
    };

    testExprs(strs);

    expectNoDiagnostics();
}

TEST_F(ExprTestFixture, ParsesBinaryExpressionsCorrectly) {
    std::vector<std::pair<std::string, Expr*>> strs1 = {
        {"1 + 2", binaryExpr(integerLiteral(1), integerLiteral(2), BinaryOp::PLUS)},
        {"1 + 2 + 3", binaryExpr(
            binaryExpr(integerLiteral(1), integerLiteral(2), BinaryOp::PLUS),
            integerLiteral(3),
            BinaryOp::PLUS)},
        {"1 + 2 * 3", binaryExpr(
            integerLiteral(1),
            binaryExpr(integerLiteral(2), integerLiteral(3), BinaryOp::STAR),
            BinaryOp::PLUS)},
        {"1 * 2 + 3", binaryExpr(
            binaryExpr(integerLiteral(1), integerLiteral(2), BinaryOp::STAR),
            integerLiteral(3),
            BinaryOp::PLUS)},
        {"1 - 2 - 3 - 4", binaryExpr(
            binaryExpr(
                binaryExpr(integerLiteral(1), integerLiteral(2), BinaryOp::MINUS),
                integerLiteral(3),
                BinaryOp::MINUS),
            integerLiteral(4),
            BinaryOp::MINUS)},
        {"1 + 2 - 3", binaryExpr(
            binaryExpr(integerLiteral(1), integerLiteral(2), BinaryOp::PLUS),
            integerLiteral(3),
            BinaryOp::MINUS)},
        {"true * abc + null", binaryExpr(
            binaryExpr(boolLiteral(true), identifierRef("abc"), BinaryOp::STAR),
            nullLiteral(),
            BinaryOp::PLUS)},
        {"a * b * c", binaryExpr(
            binaryExpr(identifierRef("a"), identifierRef("b"), BinaryOp::STAR),
            identifierRef("c"),
            BinaryOp::STAR)}
    };

    testExprs(strs1);

    expectNoDiagnostics();
}

TEST_F(ExprTestFixture, ParsesExpressionsWithParenthesisCorrectly) {
    std::vector<std::pair<std::string, Expr*>> strs = {
        {"1 + (2 + 3)", binaryExpr(
            integerLiteral(1),
            binaryExpr(integerLiteral(2), integerLiteral(3), BinaryOp::PLUS),
            BinaryOp::PLUS)},
        {"(1 + 2) * 3", binaryExpr(
            binaryExpr(integerLiteral(1), integerLiteral(2), BinaryOp::PLUS),
            integerLiteral(3),
            BinaryOp::STAR)},
        {"1 + (2 + (3 + 4))", binaryExpr(
            integerLiteral(1),
            binaryExpr(
                integerLiteral(2),
                binaryExpr(integerLiteral(3), integerLiteral(4), BinaryOp::PLUS),
                BinaryOp::PLUS),
            BinaryOp::PLUS)},
        {"((1))", integerLiteral(1)},
        {"((1) + 2)", binaryExpr(integerLiteral(1), integerLiteral(2), BinaryOp::PLUS)},
        {"1 / (2 + 3)", binaryExpr(
            integerLiteral(1),
            binaryExpr(integerLiteral(2), integerLiteral(3), BinaryOp::PLUS),
            BinaryOp::SLASH)}
    };

    testExprs(strs);

    expectNoDiagnostics();
}

TEST_F(ExprTestFixture, ParsesExpressionsWithUnaryOperatorsCorrectly) {
    std::vector<std::pair<std::string, Expr*>> strs = {
        {"+1", unaryExpr(integerLiteral(1), UnaryOp::PLUS)},
        {"-2", unaryExpr(integerLiteral(2), UnaryOp::MINUS)},
        {"!xyz", unaryExpr(identifierRef("xyz"), UnaryOp::NOT)},
        {"++x", unaryExpr(identifierRef("x"), UnaryOp::PRE_INCREMENT)},
        {"--x", unaryExpr(identifierRef("x"), UnaryOp::PRE_DECREMENT)},
        {"!(a + b)", unaryExpr(binaryExpr(
            identifierRef("a"),
            identifierRef("b"),
            BinaryOp::PLUS),
            UnaryOp::NOT)},
        {"!a + b", binaryExpr(
            unaryExpr(identifierRef("a"), UnaryOp::NOT),
            identifierRef("b"),
            BinaryOp::PLUS)},
        {"-(!a + b)", unaryExpr(parseExpr("!a + b"), UnaryOp::MINUS)},
        {"-1 + 2 + (-3)", binaryExpr(
        binaryExpr(unaryExpr(integerLiteral(1), UnaryOp::MINUS),
        integerLiteral(2),
        BinaryOp::PLUS),
        unaryExpr(integerLiteral(3), UnaryOp::MINUS),
        BinaryOp::PLUS)},
        {"++x - ++y * --z", binaryExpr(
            unaryExpr(identifierRef("x"), UnaryOp::PRE_INCREMENT), // ++x
            binaryExpr(
                unaryExpr(identifierRef("y"), UnaryOp::PRE_INCREMENT), // ++y
                unaryExpr(identifierRef("z"), UnaryOp::PRE_DECREMENT), // --z
                BinaryOp::STAR),
            BinaryOp::MINUS)},
        {"x++", unaryExpr(identifierRef("x"), UnaryOp::POST_INCREMENT)},
        {"y--", unaryExpr(identifierRef("y"), UnaryOp::POST_DECREMENT)},
        {"(x++ - !y) * --z", binaryExpr(
            binaryExpr(
            unaryExpr(identifierRef("x"), UnaryOp::POST_INCREMENT), // x++
                unaryExpr(identifierRef("y"), UnaryOp::NOT), // !y
                BinaryOp::MINUS),
            unaryExpr(identifierRef("z"), UnaryOp::PRE_DECREMENT), // --z
            BinaryOp::STAR)}
    };

    testExprs(strs);

    expectNoDiagnostics();
}

TEST_F(ExprTestFixture, ParsesIndexingExpressionsCorrectly) {
    std::vector<std::pair<std::string, Expr*>> strs = {
        {"a[]", indexExpr(identifierRef("a"), nullptr)},
        {"!a[]", unaryExpr(indexExpr(identifierRef("a"), nullptr), UnaryOp::NOT)},
        {"a[x]", indexExpr(identifierRef("a"), {identifierRef("x")})},
        {"b[x++ + 2]", indexExpr(identifierRef("b"),
            binaryExpr(unaryExpr(identifierRef("x"), UnaryOp::POST_INCREMENT), integerLiteral(2), BinaryOp::PLUS)
        )},
        {"a[x] + b[y]", binaryExpr(
            indexExpr(identifierRef("a"), {identifierRef("x")}),
            indexExpr(identifierRef("b"), {identifierRef("y")}),
            BinaryOp::PLUS)},
    };

    testExprs(strs);

    expectNoDiagnostics();
}

TEST_F(ExprTestFixture, ParsesCallExpressionCorrectly) {
    std::vector<std::pair<std::string, Expr*>> strs = {
        {"a()", callExpr(identifierRef("a"), {})},
        {"!a()", unaryExpr(callExpr(identifierRef("a"), {}), UnaryOp::NOT)},
        {"a(x)", callExpr(identifierRef("a"), {identifierRef("x")})},
        {"b(x, y++, 1, --z)", callExpr(identifierRef("b"), {
            identifierRef("x"),
            unaryExpr(identifierRef("y"), UnaryOp::POST_INCREMENT),
            integerLiteral(1),
            unaryExpr(identifierRef("z"), UnaryOp::PRE_DECREMENT)
        })},
        {"a(x) + b(y)", binaryExpr(
            callExpr(identifierRef("a"), {identifierRef("x")}),
            callExpr(identifierRef("b"), {identifierRef("y")}),
            BinaryOp::PLUS)},
        {"a(x, y) + b(x--, z)++", binaryExpr(
            callExpr(identifierRef("a"), {identifierRef("x"), identifierRef("y")}),
            unaryExpr(callExpr(identifierRef("b"), {unaryExpr(identifierRef("x"), UnaryOp::POST_DECREMENT), identifierRef("z")}), UnaryOp::POST_INCREMENT),
            BinaryOp::PLUS)}
    };

    testExprs(strs);

    expectNoDiagnostics();
}