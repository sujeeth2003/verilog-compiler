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

    // ---- top-level grammar ----
    std::vector<std::string> parsePortList() {
        std::vector<std::string> ports;
        if (check(Tok::RPAREN)) return ports; // empty port list
        ports.push_back(expect(Tok::IDENT, "expected port name").text);
        while (check(Tok::COMMA)) {
            advance();
            ports.push_back(expect(Tok::IDENT, "expected port name").text);
        }
        return ports;
    }

    void parseItem(Module& m) {
        switch (peek().type) {
            case Tok::KW_WIRE:   parseDecl(m, SigKind::WIRE);   return;
            case Tok::KW_REG:    parseDecl(m, SigKind::REG);    return;
            case Tok::KW_INPUT:  parseDecl(m, SigKind::INPUT);  return;
            case Tok::KW_OUTPUT: parseDecl(m, SigKind::OUTPUT); return;
            case Tok::KW_ASSIGN: parseAssign(m);                return;
            default:
                throw std::runtime_error("Parse error line " + std::to_string(peek().line) +
                                          ": unexpected token '" + peek().text + "' in module body");
        }
    }

    void parseDecl(Module& m, SigKind kind) {
        advance(); // consume the keyword
        m.decls.push_back({kind, expect(Tok::IDENT, "expected signal name").text});
        while (check(Tok::COMMA)) {
            advance();
            m.decls.push_back({kind, expect(Tok::IDENT, "expected signal name").text});
        }
        expect(Tok::SEMI, "expected ';' after declaration");
    }

    void parseAssign(Module& m) {
        advance(); // consume 'assign'
        std::string target = expect(Tok::IDENT, "expected target signal").text;
        expect(Tok::EQUALS, "expected '=' in assign");
        ExprPtr e = parseExpr();
        expect(Tok::SEMI, "expected ';' after assign");
        m.assigns.push_back({target, e});
    }

    // ---- expression grammar, lowest to highest precedence ----
    ExprPtr parseExpr()  { return parseOr(); }

    ExprPtr parseOr() {
        ExprPtr lhs = parseXor();
        while (check(Tok::PIPE)) { advance(); lhs = makeBinOp(BinOp::OR, lhs, parseXor()); }
        return lhs;
    }
    ExprPtr parseXor() {
        ExprPtr lhs = parseAnd();
        while (check(Tok::CARET)) { advance(); lhs = makeBinOp(BinOp::XOR, lhs, parseAnd()); }
        return lhs;
    }
    ExprPtr parseAnd() {
        ExprPtr lhs = parseAdd();
        while (check(Tok::AMP)) { advance(); lhs = makeBinOp(BinOp::AND, lhs, parseAdd()); }
        return lhs;
    }
    ExprPtr parseAdd() {
        ExprPtr lhs = parseUnary();
        while (check(Tok::PLUS) || check(Tok::MINUS)) {
            BinOp op = check(Tok::PLUS) ? BinOp::ADD : BinOp::SUB;
            advance();
            lhs = makeBinOp(op, lhs, parseUnary());
        }
        return lhs;
    }
    ExprPtr parseUnary() {
        if (check(Tok::TILDE)) { advance(); return makeUnOp(UnOp::NOT, parseUnary()); }
        return parsePrimary();
    }
    ExprPtr parsePrimary() {
        if (check(Tok::IDENT))  return makeIdent(advance().text);
        if (check(Tok::NUMBER)) return makeNumber(parseNumberLiteral(advance().text));
        if (check(Tok::LPAREN)) {
            advance();
            ExprPtr e = parseExpr();
            expect(Tok::RPAREN, "expected ')'");
            return e;
        }
        throw std::runtime_error("Parse error line " + std::to_string(peek().line) +
                                  ": expected expression, got '" + peek().text + "'");
    }

    // Handles plain decimal ("42") and a simplified Verilog literal ("4'b1010", "8'hFF").
    long parseNumberLiteral(const std::string& raw) {
        auto tick = raw.find('\'');
        if (tick == std::string::npos)
            return std::stol(raw);
        std::string digits = raw.substr(tick + 2); // skip "'b" / "'h" / "'d"
        char base = raw[tick + 1];
        int radix = (base == 'b') ? 2 : (base == 'h') ? 16 : 10;
        return std::stol(digits, nullptr, radix);
    }
};
