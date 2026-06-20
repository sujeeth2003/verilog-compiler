// parser.hpp — Stage 3: recursive-descent parser, tokens -> AST.
//
// Grammar handled (subset of Verilog):
//
//   module     := 'module' IDENT '(' portlist ')' ';' item* 'endmodule'
//   portlist   := IDENT (',' IDENT)*
//   item       := decl | assign
//   decl       := ('wire'|'reg'|'input'|'output') IDENT (',' IDENT)* ';'
//   assign     := 'assign' IDENT '=' expr ';'
//
//   expr       := orExpr
//   orExpr     := xorExpr ('|' xorExpr)*
//   xorExpr    := andExpr ('^' andExpr)*
//   andExpr    := addExpr ('&' addExpr)*
//   addExpr    := unary (('+'|'-') unary)*
//   unary      := '~' unary | primary
//   primary    := IDENT | NUMBER | '(' expr ')'
#pragma once
#include "lexer.hpp"
#include "ast.hpp"
#include <stdexcept>

