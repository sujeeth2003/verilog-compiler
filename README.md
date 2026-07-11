# vcomp — a tiny Verilog-subset compiler

Build:
    ./build.sh

Run:
    ./vcomp example_mux.v --ast          # parse tree + gate-level netlist
    ./vcomp example_mux.v --dot | dot -Tpng -o mux.png     # Graphviz schematic (example_mux.dot is checked in)

## Pipeline (src/main.cpp ties these together, in order)
1. `lexer.hpp`      — text -> tokens
2. `ast.hpp`        — tree node definitions
3. `parser.hpp`     — tokens -> AST (recursive descent, one function per grammar rule)
4. `ast_print.hpp`  — debug printer for the AST
5. `netlist.hpp`    — AST -> flat gate-level netlist (the "codegen" phase)

