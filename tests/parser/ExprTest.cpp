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

TEST_F(ExprTestFixture, ParsesBasicBinaryExpressionsCorrectly) {
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
            BinaryOp::PLUS)}
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
            BinaryOp::PLUS)},
        {"a[x] + b(y)", binaryExpr(
            indexExpr(identifierRef("a"), identifierRef("x")),
            callExpr(identifierRef("b"), {identifierRef("y")}),
            BinaryOp::PLUS)},
        {"a[x + b(y) * 2] + 1", binaryExpr(
            indexExpr(
                identifierRef("a"),
                binaryExpr(
                    identifierRef("x"),
                    binaryExpr(callExpr(identifierRef("b"), {identifierRef("y")}), integerLiteral(2), BinaryOp::STAR),
                    BinaryOp::PLUS)),
             integerLiteral(1),
             BinaryOp::PLUS
            )}
    };

    testExprs(strs);

    expectNoDiagnostics();
}

TEST_F(ExprTestFixture, ParsesMemberAccessCorrectly) {
    std::vector<std::pair<std::string, Expr*>> strs = {
        {"a.b", memberAccessExpr(identifierRef("a"), "b")},
        {"a.c()", callExpr(
            memberAccessExpr(identifierRef("a"), "c"),
            {})},
        {"a.b().c", memberAccessExpr(
            callExpr(memberAccessExpr(identifierRef("a"), "b"), {}),
            "c")},
        {"a.b* c.d", binaryExpr(
            memberAccessExpr(identifierRef("a"), "b"),
            memberAccessExpr(identifierRef("c"), "d"),
            BinaryOp::STAR)},
        {"(a+b).c", memberAccessExpr(
            binaryExpr(identifierRef("a"), identifierRef("b"), BinaryOp::PLUS),
            "c")},
        {"a(b.c())", callExpr(
            identifierRef("a"), {callExpr(memberAccessExpr(identifierRef("b"), "c"),
                {})})},
        {"a.b.c + d(e.f, g.h)", binaryExpr(
            memberAccessExpr(memberAccessExpr(identifierRef("a"), "b"), "c"),
            callExpr(identifierRef("d"), {
                memberAccessExpr(identifierRef("e"), "f"),
                memberAccessExpr(identifierRef("g"), "h")
            }),
            BinaryOp::PLUS)}
    };

    testExprs(strs);

    expectNoDiagnostics();
}

TEST_F(ExprTestFixture, ParsesTernaryOperatorCorrectly) {
    std::vector<std::pair<std::string, Expr*>> strs = {
        {"true ? 1 : 2", ternaryExpr(boolLiteral(true), integerLiteral(1), integerLiteral(2))},
        {"a.b ? c.d : 1", ternaryExpr(memberAccessExpr(identifierRef("a"), "b"), memberAccessExpr(identifierRef("c"), "d"), integerLiteral(1))}
    };

    testExprs(strs);

    expectNoDiagnostics();
}

TEST_F(ExprTestFixture, ParsesAssociativityCorrectly) {
    std::vector leftAssociativeOps = {
        BinaryOp::LOGICAL_OR,
        BinaryOp::LOGICAL_AND,
        BinaryOp::BITWISE_OR,
        BinaryOp::BITWISE_XOR,
        BinaryOp::BITWISE_AND,
        BinaryOp::DOUBLE_EQUALS,
        BinaryOp::NOT_EQUALS,
        BinaryOp::LESS,
        BinaryOp::GREATER,
        BinaryOp::LESS_EQUALS,
        BinaryOp::GREATER_EQUALS,
        BinaryOp::LEFT_SHIFT,
        BinaryOp::RIGHT_SHIFT,
        BinaryOp::PLUS,
        BinaryOp::MINUS,
        BinaryOp::STAR,
        BinaryOp::SLASH,
        BinaryOp::MODULO
    };
    std::vector rightAssociativeOps = {
        BinaryOp::EQUALS,
        BinaryOp::PLUS_EQUALS,
        BinaryOp::MINUS_EQUALS,
        BinaryOp::STAR_EQUALS,
        BinaryOp::SLASH_EQUALS,
        BinaryOp::MODULO_EQUALS,
        BinaryOp::LEFT_SHIFT_EQUALS,
        BinaryOp::RIGHT_SHIFT_EQUALS,
        BinaryOp::BITWISE_AND_EQUALS,
        BinaryOp::BITWISE_OR_EQUALS,
        BinaryOp::BITWISE_XOR_EQUALS
    };

    // TODO - Ternary Operator

    std::vector<std::pair<std::string, Expr*>> leftAssociativeStrs;
    leftAssociativeStrs.reserve(leftAssociativeOps.size());

    for (auto op: leftAssociativeOps) {
        // (1 op 2 op 3) == ((1 op 2) op 3)
        auto expr = binaryExpr(binaryExpr(integerLiteral(1), integerLiteral(2), op), integerLiteral(3), op);
        std::string str_op = tokenTypeToString(binaryOpToTokenType(op));
        leftAssociativeStrs.emplace_back("1 " + str_op + " 2 " + str_op + " 3", expr);
    }

    // a.b.c == (a.b).c
    leftAssociativeStrs.emplace_back("a.b.c", memberAccessExpr(memberAccessExpr(identifierRef("a"), "b"), "c"));

    testExprs(leftAssociativeStrs);
    expectNoDiagnostics();

    std::vector<std::pair<std::string, Expr*>> rightAssociativeStrs;
    rightAssociativeStrs.reserve(rightAssociativeOps.size());

    for (auto op: rightAssociativeOps) {
        // (1 op 2 op 3) == (1 op (2 op 3))
        auto expr = binaryExpr(integerLiteral(1), binaryExpr(integerLiteral(2), integerLiteral(3), op), op);
        std::string str_op = tokenTypeToString(binaryOpToTokenType(op));
        rightAssociativeStrs.emplace_back("1 " + str_op + " 2 " + str_op + " 3", expr);
    }

    testExprs(rightAssociativeStrs);
    expectNoDiagnostics();
}

TEST_F(ExprTestFixture, ParsesInfixExpressionsWithCorrectBinding) {
    std::vector<std::vector<BinaryOp>> ops = {
        // Level 0: Assignment operators (lowest precedence, binding power 2,1)
        {BinaryOp::EQUALS, BinaryOp::PLUS_EQUALS, BinaryOp::MINUS_EQUALS, BinaryOp::STAR_EQUALS, 
         BinaryOp::SLASH_EQUALS, BinaryOp::MODULO_EQUALS, BinaryOp::LEFT_SHIFT_EQUALS, 
         BinaryOp::RIGHT_SHIFT_EQUALS, BinaryOp::BITWISE_AND_EQUALS, BinaryOp::BITWISE_OR_EQUALS, 
         BinaryOp::BITWISE_XOR_EQUALS},
        
        // Level 1: Logical OR (binding power 5,6)
        {BinaryOp::LOGICAL_OR},
        
        // Level 2: Logical AND (binding power 7,8)
        {BinaryOp::LOGICAL_AND},
        
        // Level 3: Bitwise OR (binding power 9,10)
        {BinaryOp::BITWISE_OR},
        
        // Level 4: Bitwise XOR (binding power 11,12)
        {BinaryOp::BITWISE_XOR},
        
        // Level 5: Bitwise AND (binding power 13,14)
        {BinaryOp::BITWISE_AND},
        
        // Level 6: Equality operators (binding power 15,16)
        {BinaryOp::DOUBLE_EQUALS, BinaryOp::NOT_EQUALS},
        
        // Level 7: Relational operators (binding power 17,18)
        {BinaryOp::LESS, BinaryOp::GREATER, BinaryOp::LESS_EQUALS, BinaryOp::GREATER_EQUALS},
        
        // Level 8: Shift operators (binding power 19,20)
        {BinaryOp::LEFT_SHIFT, BinaryOp::RIGHT_SHIFT},
        
        // Level 9: Additive operators (binding power 21,22)
        {BinaryOp::PLUS, BinaryOp::MINUS},
        
        // Level 10: Multiplicative operators (binding power 23,24) - highest precedence
        {BinaryOp::STAR, BinaryOp::SLASH, BinaryOp::MODULO}
    };

    std::vector<std::pair<std::string, Expr*>> strs;

    std::unordered_map<BinaryOp, int> power;
    int level = 0;
    for (auto& opLevel: ops) {
        for (auto op: opLevel) power[op] = level;
        level++;
    }

    for (auto [op1, level1]: power) {
        for (auto [op2, level2]: power) {
            if (op1 == op2 || level1 == level2) continue;

            auto winner_op = level1 > level2 ? op1 : op2;
            auto other_op = level1 > level2 ? op2 : op1;
            std::string str_winner = tokenTypeToString(binaryOpToTokenType(winner_op));
            std::string str_other = tokenTypeToString(binaryOpToTokenType(other_op));

            // 1 winner_op 2 other_op 3 == (1 winner_op 2) other_op 3
            auto expr1 = binaryExpr(binaryExpr(integerLiteral(1), integerLiteral(2), winner_op), integerLiteral(3), other_op);

            // 1 other_op 2 winner_op 3 == 1 other_op (2 winner_op 3)
            auto expr2 = binaryExpr(integerLiteral(1), binaryExpr(integerLiteral(2), integerLiteral(3), winner_op), other_op);

            strs.emplace_back("1 " + str_winner + " 2 " + str_other + " 3", expr1);
            strs.emplace_back("1 " + str_other + " 2 " + str_winner + " 3", expr2);
        }
    }

    for (const auto op: power | std::views::keys) {
        std::string str_op = tokenTypeToString(binaryOpToTokenType(op));
        // a . b op c == (a.b) op c
        auto expr1 = binaryExpr(memberAccessExpr(identifierRef("a"), "b"), identifierRef("c"), op);

        // a op b . c == a op (b.c)
        auto expr2 = binaryExpr(identifierRef("a"), memberAccessExpr(identifierRef("b"), "c"), op);

        strs.emplace_back("a . b " + str_op + " c", expr1);
        strs.emplace_back("a " + str_op + " b . c", expr2);
    }

    testExprs(strs);
    expectNoDiagnostics();
}

TEST_F(ExprTestFixture, ParsesInfixExpressionsWithSameBindingPowerWithCorrectAssociativity) {
    std::vector<std::vector<BinaryOp>> right_associative_ops = {
        {BinaryOp::EQUALS, BinaryOp::PLUS_EQUALS, BinaryOp::MINUS_EQUALS, BinaryOp::STAR_EQUALS,
         BinaryOp::SLASH_EQUALS, BinaryOp::MODULO_EQUALS, BinaryOp::LEFT_SHIFT_EQUALS,
         BinaryOp::RIGHT_SHIFT_EQUALS, BinaryOp::BITWISE_AND_EQUALS, BinaryOp::BITWISE_OR_EQUALS,
         BinaryOp::BITWISE_XOR_EQUALS},
    };

    std::vector<std::vector<BinaryOp>> left_associative_ops = {
        {BinaryOp::DOUBLE_EQUALS, BinaryOp::NOT_EQUALS},
        {BinaryOp::LESS, BinaryOp::GREATER, BinaryOp::LESS_EQUALS, BinaryOp::GREATER_EQUALS},
        {BinaryOp::LEFT_SHIFT, BinaryOp::RIGHT_SHIFT},
        {BinaryOp::PLUS, BinaryOp::MINUS},
        {BinaryOp::STAR, BinaryOp::SLASH, BinaryOp::MODULO}
    };

    std::vector<std::pair<std::string, Expr*>> strs;

    for (auto& ops: right_associative_ops) {
        for (const auto op1: ops) {
            for (const auto op2: ops) {
                std::string str_op1 = tokenTypeToString(binaryOpToTokenType(op1));
                std::string str_op2 = tokenTypeToString(binaryOpToTokenType(op2));

                // 1 op1 2 op2 3 == 1 op1 (2 op2 3)
                auto expr1 = binaryExpr(integerLiteral(1), binaryExpr(integerLiteral(2), integerLiteral(3), op2), op1);

                // 1 op2 2 op1 3 == 1 op2 (2 op1 3)
                auto expr2 = binaryExpr(integerLiteral(1), binaryExpr(integerLiteral(2), integerLiteral(3), op1), op2);

                strs.emplace_back("1 " + str_op1 + " 2 " + str_op2 + " 3", expr1);
                strs.emplace_back("1 " + str_op2 + " 2 " + str_op1 + " 3", expr2);
            }
        }
    }

    for (auto& ops: left_associative_ops) {
        for (const auto op1: ops) {
            for (const auto op2: ops) {
                std::string str_op1 = tokenTypeToString(binaryOpToTokenType(op1));
                std::string str_op2 = tokenTypeToString(binaryOpToTokenType(op2));

                // 1 op1 2 op2 3 == (1 op1 2) op2 3
                auto expr1 = binaryExpr(binaryExpr(integerLiteral(1), integerLiteral(2), op1), integerLiteral(3), op2);

                // 1 op2 2 op1 3 == (1 op2 2) op1 3
                auto expr2 = binaryExpr(binaryExpr(integerLiteral(1), integerLiteral(2), op2), integerLiteral(3), op1);

                strs.emplace_back("1 " + str_op1 + " 2 " + str_op2 + " 3", expr1);
                strs.emplace_back("1 " + str_op2 + " 2 " + str_op1 + " 3", expr2);
            }
        }
    }

    testExprs(strs);
    expectNoDiagnostics();
}

TEST_F(ExprTestFixture, ParsesPostfixWithGreaterBindingPowerThanPrefixWhichIsStrongerThanInfix) {
    std::vector<std::pair<std::string, Expr*>> strs;

    // Infix vs Prefix and Postfix
    for (auto op: magic_enum::enum_values<BinaryOp>()) {
        std::string str_op = tokenTypeToString(binaryOpToTokenType(op));

        // prefix
        for (auto unaryOp: magic_enum::enum_values<UnaryOp>()) {
            if (unaryOp == UnaryOp::POST_INCREMENT || unaryOp == UnaryOp::POST_DECREMENT) continue;
            std::string str_unary_op = ::toString(unaryOp);

            // a op unaryOp b == a op (unaryOp b)
            strs.emplace_back("a " + str_op + " " + str_unary_op + " b", binaryExpr(identifierRef("a"), unaryExpr(identifierRef("b"), unaryOp), op));
            // unaryOp a op b == (unaryOp a) op b
            strs.emplace_back(str_unary_op + " a " + str_op + " b", binaryExpr(unaryExpr(identifierRef("a"), unaryOp), identifierRef("b"), op));
        }

        // postfix

        for (auto tokenType: std::vector{TokenType::LEFT_PAREN, TokenType::LEFT_BRACKET, TokenType::INCREMENT, TokenType::DECREMENT}) {
            std::string str_postfix_op;
            switch (tokenType) {
                case TokenType::LEFT_PAREN:
                    str_postfix_op = "(0)";
                    break;
                case TokenType::LEFT_BRACKET:
                    str_postfix_op = "[0]";
                    break;
                default:
                    str_postfix_op = tokenTypeToString(tokenType);
            }

            Expr* expr1 = nullptr;
            Expr* expr2 = nullptr;
            switch (tokenType) {
                case TokenType::LEFT_PAREN:
                    expr1 = callExpr(identifierRef("b"), {integerLiteral(0)});
                    expr2 = callExpr(identifierRef("a"), {integerLiteral(0)});
                    break;
                case TokenType::LEFT_BRACKET:
                    expr1 = indexExpr(identifierRef("b"),  integerLiteral(0));
                    expr2 = indexExpr(identifierRef("a"),  integerLiteral(0));
                    break;
                case TokenType::INCREMENT:
                    expr1 = unaryExpr(identifierRef("b"), UnaryOp::POST_INCREMENT);
                    expr2 = unaryExpr(identifierRef("a"), UnaryOp::POST_INCREMENT);
                    break;
                case TokenType::DECREMENT:
                    expr1 = unaryExpr(identifierRef("b"), UnaryOp::POST_DECREMENT);
                    expr2 = unaryExpr(identifierRef("a"), UnaryOp::POST_DECREMENT);
                    break;
                default:
                    // Unreachable
            }

            // a op b postfix == a op (b postfix)
            strs.emplace_back("a " + str_op + " b " + str_postfix_op, binaryExpr(identifierRef("a"), expr1, op));
            // a postfix op b == (a postfix) op b
            strs.emplace_back("a " + str_postfix_op + " " + str_op + " b", binaryExpr(expr2, identifierRef("b"), op));
        }
    }

    // TODO - Postfix vs Prefix vs Member Access

    testExprs(strs);
    expectNoDiagnostics();
}