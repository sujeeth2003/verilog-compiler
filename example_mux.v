module mux2(sel, a, b, y);
  input sel, a, b;
  output y;
  wire n_sel;
  assign n_sel = ~sel;
  assign y = (a & n_sel) | (b & sel);
endmodule
