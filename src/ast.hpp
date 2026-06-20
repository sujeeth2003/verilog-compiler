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

inline ExprPtr makeIdent(const std::string& n) {
    auto e = std::make_shared<Expr>(); e->kind = ExprKind::IDENT; e->name = n; return e;
}
inline ExprPtr makeNumber(long v) {
    auto e = std::make_shared<Expr>(); e->kind = ExprKind::NUMBER; e->value = v; return e;
}
inline ExprPtr makeBinOp(BinOp op, ExprPtr l, ExprPtr r) {
    auto e = std::make_shared<Expr>(); e->kind = ExprKind::BINOP; e->bop = op; e->lhs = l; e->rhs = r; return e;
}
inline ExprPtr makeUnOp(UnOp op, ExprPtr o) {
    auto e = std::make_shared<Expr>(); e->kind = ExprKind::UNOP; e->uop = op; e->operand = o; return e;
}

// ---- Statements ----
