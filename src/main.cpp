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

