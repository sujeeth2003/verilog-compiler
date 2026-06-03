// lexer.hpp — Stage 1: turn Verilog source text into a stream of tokens.
#pragma once
#include <string>
#include <vector>
#include <cctype>
#include <stdexcept>

enum class Tok {
    END, IDENT, NUMBER,
    KW_MODULE, KW_ENDMODULE, KW_WIRE, KW_REG, KW_INPUT, KW_OUTPUT, KW_ASSIGN,
    LPAREN, RPAREN, LBRACK, RBRACK, SEMI, COMMA, EQUALS,
    PLUS, MINUS, AMP, PIPE, CARET, TILDE
};

struct Token {
    Tok type;
    std::string text;   // raw text (identifier name, number literal, etc.)
    int line;
};

// Turns a full source string into a vector of tokens (one pass, no lookback).
class Lexer {
public:
    explicit Lexer(const std::string& src) : s(src) {}

    std::vector<Token> tokenize() {
        std::vector<Token> out;
        while (true) {
            Token t = next();
            out.push_back(t);
            if (t.type == Tok::END) break;
        }
        return out;
    }

