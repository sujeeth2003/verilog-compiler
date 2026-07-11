# vcomp — a tiny Verilog-subset compiler

Build:
    ./build.sh

Run:
    ./vcomp example_mux.v --ast          # parse tree + gate-level netlist
    ./vcomp example_mux.v --dot | dot -Tpng -o mux.png     # Graphviz schematic (example_mux.dot is checked in)

