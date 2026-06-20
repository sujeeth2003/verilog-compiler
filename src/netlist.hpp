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

