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

## Supported subset
- `module NAME(ports...); ... endmodule`
- `input`, `output`, `wire`, `reg` declarations
- `assign target = expr;`
- Operators: `& | ^ ~ + -`, parentheses, decimal and `N'bXXXX` / `N'hXX` literals

## Where to extend next
- Add `always @(...)` blocks -> needs a statement AST + simple scheduler
- Add bit-width tracking ([7:0] etc.) -> extend SignalDecl with a width field
- Add multi-bit constant folding / optimization pass over the netlist
- Emit real Verilog/BLIF/structural output instead of the text netlist
- Add a symbol table pass between parsing and netlist emission to catch
  undeclared signals (currently the emitter trusts names blindly)

## Error messages
Syntax errors report the source line, e.g. `vcomp: error: Parse error line 1: expected ')' after port list (got ';')`.
