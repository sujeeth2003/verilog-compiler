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

class Parser {
public:
    explicit Parser(std::vector<Token> toks) : t(std::move(toks)) {}

    Module parseModule() {
        Module m;
        expect(Tok::KW_MODULE, "expected 'module'");
        m.name = expect(Tok::IDENT, "expected module name").text;
        expect(Tok::LPAREN, "expected '(' after module name");
        m.ports = parsePortList();
        expect(Tok::RPAREN, "expected ')' after port list");
        expect(Tok::SEMI, "expected ';' after module header");

        while (!check(Tok::KW_ENDMODULE))
            parseItem(m);

        expect(Tok::KW_ENDMODULE, "expected 'endmodule'");
        return m;
    }

private:
    std::vector<Token> t;
    size_t pos = 0;

    // ---- token stream helpers ----
    const Token& peek() { return t[pos]; }
    bool check(Tok k) { return peek().type == k; }
    const Token& advance() { return t[pos++]; }
    const Token& expect(Tok k, const std::string& msg) {
        if (!check(k))
            throw std::runtime_error("Parse error line " + std::to_string(peek().line) +
                                      ": " + msg + " (got '" + peek().text + "')");
        return advance();
    }

