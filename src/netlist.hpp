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
        }
        return ""; // unreachable
    }
};

inline void printNetlist(const std::vector<Gate>& gates) {
    for (auto& g : gates) {
        switch (g.op) {
            case GateOp::AND:  std::cout << g.out << " = AND(" << g.in1 << ", " << g.in2 << ")\n"; break;
            case GateOp::OR:   std::cout << g.out << " = OR("  << g.in1 << ", " << g.in2 << ")\n"; break;
            case GateOp::XOR:  std::cout << g.out << " = XOR(" << g.in1 << ", " << g.in2 << ")\n"; break;
            case GateOp::ADD:  std::cout << g.out << " = ADD(" << g.in1 << ", " << g.in2 << ")\n"; break;
            case GateOp::SUB:  std::cout << g.out << " = SUB(" << g.in1 << ", " << g.in2 << ")\n"; break;
            case GateOp::NOT:  std::cout << g.out << " = NOT(" << g.in1 << ")\n"; break;
            case GateOp::BUF:  std::cout << g.out << " = BUF(" << g.in1 << ")\n"; break;
            case GateOp::CONST:std::cout << g.out << " = CONST(" << g.constVal << ")\n"; break;
        }
    }
}

// ---- Graphviz schematic output --------------------------------------------
// `vcomp file.v --dot > out.dot && dot -Tpng out.dot -o out.png`
// Inputs are green boxes, outputs blue boxes, each gate an ellipse, every connection an edge from
// the driver to the consumer labelled with the net name.
inline const char* gateName(GateOp op) {
    switch (op) {
        case GateOp::AND: return "AND"; case GateOp::OR: return "OR"; case GateOp::XOR: return "XOR";
        case GateOp::NOT: return "NOT"; case GateOp::ADD: return "ADD"; case GateOp::SUB: return "SUB";
        case GateOp::BUF: return "BUF"; case GateOp::CONST: return "CONST";
    }
    return "?";
}

inline void printGraphviz(const Module& m, const std::vector<Gate>& gates, std::ostream& os) {
    std::set<std::string> inputs, outputs;
    for (auto& d : m.decls) {
        if (d.kind == SigKind::INPUT) inputs.insert(d.name);
        if (d.kind == SigKind::OUTPUT) outputs.insert(d.name);
    }
    // A BUF whose target is a named signal just ties a computed temp to that name: draw it as a wire.
    auto isTie = [](const Gate& g) { return g.op == GateOp::BUF && !g.out.empty() && g.out[0] != '_'; };
    // Map every named signal driven by a tie to the temp that actually computes it.
    std::map<std::string, std::string> alias;
    for (auto& g : gates) if (isTie(g)) alias[g.out] = g.in1;
    auto resolve = [&](std::string net) {
        while (alias.count(net) && alias[net] != net) net = alias[net];
        return net;
    };
    auto producer = [&](const std::string& net) {
        std::string n = resolve(net);
        return inputs.count(n) ? "in_" + n : "g_" + n;
    };

    os << "digraph \"" << m.name << "\" {\n  rankdir=LR;\n  node [fontname=\"Helvetica\"];\n";
    for (auto& i : inputs)
        os << "  \"in_" << i << "\" [label=\"" << i << "\", shape=box, style=filled, fillcolor=\"#c8e6c9\"];\n";
    for (auto& o : outputs)
        os << "  \"out_" << o << "\" [label=\"" << o << "\", shape=box, style=filled, fillcolor=\"#bbdefb\"];\n";
    for (auto& g : gates) {
        if (isTie(g)) continue;
        os << "  \"g_" << g.out << "\" [label=\"" << gateName(g.op);
        if (g.op == GateOp::CONST) os << "\\n" << g.constVal;
        os << "\", shape=ellipse];\n";
    }
    for (auto& g : gates) {
        if (isTie(g)) continue;
        for (const std::string& in : {g.in1, g.in2}) {
            if (in.empty()) continue;
            os << "  \"" << producer(in) << "\" -> \"g_" << g.out << "\" [label=\"" << in << "\"];\n";
        }
    }
    for (auto& g : gates) {       // connect each output port to whatever computes it
        if (!isTie(g) || !outputs.count(g.out)) continue;
        os << "  \"" << producer(g.in1) << "\" -> \"out_" << g.out << "\" [label=\"" << g.out << "\"];\n";
    }
    os << "}\n";
}
