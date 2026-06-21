// netlist.hpp — Stage 4: turn the AST into a flat list of gates ("netlist").
//
// Each gate has an opcode, one or two input nets, and one output net.
// Compound expressions are broken into a chain of gates using fresh
// "temp" wire names (_t0, _t1, ...), the same trick real synthesis
// tools use when lowering an expression tree to gate-level logic.
#pragma once
#include "ast.hpp"
#include <string>
#include <vector>
#include <iostream>
#include <map>
#include <set>

enum class GateOp { AND, OR, XOR, NOT, ADD, SUB, BUF, CONST };

struct Gate {
    GateOp op;
    std::string out;
    std::string in1;
    std::string in2;      // unused for NOT/BUF/CONST
    long constVal = 0;    // only used for CONST
};

class NetlistEmitter {
public:
    std::vector<Gate> emit(const Module& m) {
        gates.clear();
        tempCount = 0;
        for (auto& a : m.assigns) {
            std::string resultNet = emitExpr(a.expr);
            // final BUF ties the computed temp net to the real target signal
            gates.push_back({GateOp::BUF, a.target, resultNet, "", 0});
        }
        return gates;
    }

private:
    std::vector<Gate> gates;
    int tempCount = 0;

    std::string freshTemp() { return "_t" + std::to_string(tempCount++); }

    // Recursively lowers an expression tree, returning the net name that
    // holds its result. This is the classic "emit and return a handle"
    // pattern used in almost every real code generator.
    std::string emitExpr(const ExprPtr& e) {
        switch (e->kind) {
            case ExprKind::IDENT:
                return e->name; // already a net, no gate needed

            case ExprKind::NUMBER: {
                std::string t = freshTemp();
                gates.push_back({GateOp::CONST, t, "", "", e->value});
                return t;
            }
            case ExprKind::UNOP: {
                std::string in = emitExpr(e->operand);
                std::string t = freshTemp();
                gates.push_back({GateOp::NOT, t, in, "", 0});
                return t;
            }
            case ExprKind::BINOP: {
                std::string l = emitExpr(e->lhs);
                std::string r = emitExpr(e->rhs);
                std::string t = freshTemp();
                GateOp op = e->bop == BinOp::AND ? GateOp::AND :
                            e->bop == BinOp::OR  ? GateOp::OR  :
                            e->bop == BinOp::XOR ? GateOp::XOR :
                            e->bop == BinOp::ADD ? GateOp::ADD : GateOp::SUB;
                gates.push_back({op, t, l, r, 0});
                return t;
            }
