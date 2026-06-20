// ast_print.hpp — small helper to render the AST as text, for debugging.
#pragma once
#include "ast.hpp"
#include <iostream>

inline void printExpr(const ExprPtr& e) {
    switch (e->kind) {
        case ExprKind::IDENT:  std::cout << e->name; return;
        case ExprKind::NUMBER: std::cout << e->value; return;
        case ExprKind::UNOP:
            std::cout << "~"; printExpr(e->operand); return;
        case ExprKind::BINOP: {
            const char* sym = e->bop == BinOp::AND ? "&" :
                               e->bop == BinOp::OR  ? "|" :
                               e->bop == BinOp::XOR ? "^" :
                               e->bop == BinOp::ADD ? "+" : "-";
            std::cout << "(";
            printExpr(e->lhs);
            std::cout << " " << sym << " ";
            printExpr(e->rhs);
            std::cout << ")";
            return;
        }
    }
}

inline void printModule(const Module& m) {
    std::cout << "module " << m.name << "(";
    for (size_t i = 0; i < m.ports.size(); i++)
        std::cout << (i ? ", " : "") << m.ports[i];
    std::cout << ")\n";

    for (auto& d : m.decls) {
        const char* k = d.kind == SigKind::WIRE ? "wire" :
                         d.kind == SigKind::REG  ? "reg"  :
                         d.kind == SigKind::INPUT ? "input" : "output";
        std::cout << "  decl " << k << " " << d.name << "\n";
    }
    for (auto& a : m.assigns) {
        std::cout << "  assign " << a.target << " = ";
        printExpr(a.expr);
        std::cout << "\n";
    }
    std::cout << "endmodule\n";
}
