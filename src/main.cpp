// main.cpp — Stage 5: the compiler driver. Reads a .v file, runs all
// phases in order, prints AST (optional) and the resulting netlist.
//
// Usage: vcomp file.v [--ast] [--dot]
//   --ast   print the parse tree first
//   --dot   print a Graphviz schematic of the netlist instead of the text netlist
#include "lexer.hpp"
#include "parser.hpp"
#include "ast_print.hpp"
#include "netlist.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

static std::string readFile(const std::string& path) {
    std::ifstream in(path);
    if (!in) throw std::runtime_error("Cannot open file: " + path);
    std::ostringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

