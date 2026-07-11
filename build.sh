#!/bin/bash
# Builds the vcomp compiler.
set -e
g++ -std=c++17 -Wall -O2 -o vcomp src/main.cpp
echo "Built ./vcomp"
echo "Try: ./vcomp example_mux.v --ast"
