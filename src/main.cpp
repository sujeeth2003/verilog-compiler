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

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <file.v> [--ast] [--dot]\n";
        return 1;
    }
    std::string path = argv[1];
    bool showAst = false, dot = false;
    for (int i = 2; i < argc; ++i) {
        std::string a = argv[i];
        if (a == "--ast") showAst = true; else if (a == "--dot") dot = true;
        else { std::cerr << "vcomp: unknown option " << a << "\n"; return 1; }
    }

    try {
        std::string src = readFile(path);

        Lexer lexer(src);
        auto tokens = lexer.tokenize();

        Parser parser(tokens);
        Module mod = parser.parseModule();

        if (showAst && !dot) {
            std::cout << "=== AST ===\n";
            printModule(mod);
            std::cout << "\n";
        }

        NetlistEmitter emitter;
        auto gates = emitter.emit(mod);

