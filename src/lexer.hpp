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

