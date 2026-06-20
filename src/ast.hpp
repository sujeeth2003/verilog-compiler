// ast.hpp — Stage 2: the tree shapes the parser will build.
#pragma once
#include <string>
#include <vector>
#include <memory>

// ---- Expressions ----
enum class ExprKind { IDENT, NUMBER, BINOP, UNOP };
enum class BinOp { AND, OR, XOR, ADD, SUB };
enum class UnOp { NOT };

struct Expr {
    ExprKind kind;

    // IDENT
    std::string name;

    // NUMBER
    long value = 0;

    // BINOP
    BinOp bop{};
    std::shared_ptr<Expr> lhs, rhs;

    // UNOP
    UnOp uop{};
    std::shared_ptr<Expr> operand;
};

using ExprPtr = std::shared_ptr<Expr>;

