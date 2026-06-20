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

private:
    const std::string& s;
    size_t pos = 0;
    int line = 1;

    char peek() { return pos < s.size() ? s[pos] : '\0'; }
    char advance() { char c = s[pos++]; if (c == '\n') line++; return c; }

    void skipWhitespaceAndComments() {
        for (;;) {
            while (pos < s.size() && std::isspace((unsigned char)peek())) advance();
            if (peek() == '/' && pos + 1 < s.size() && s[pos+1] == '/') {
                while (pos < s.size() && peek() != '\n') advance();
                continue;
            }
            if (peek() == '/' && pos + 1 < s.size() && s[pos+1] == '*') {
                advance(); advance();
                while (pos < s.size() && !(peek() == '*' && pos+1 < s.size() && s[pos+1] == '/')) advance();
                if (pos < s.size()) { advance(); advance(); }
                continue;
            }
            break;
        }
    }

    Token makeSingle(Tok type) {
        Token t{type, std::string(1, advance()), line};
        return t;
    }

    Token next() {
        skipWhitespaceAndComments();
        if (pos >= s.size()) return Token{Tok::END, "", line};

        char c = peek();

        if (std::isalpha((unsigned char)c) || c == '_') {
            std::string id;
            while (pos < s.size() && (std::isalnum((unsigned char)peek()) || peek() == '_'))
                id += advance();
            if (id == "module") return {Tok::KW_MODULE, id, line};
            if (id == "endmodule") return {Tok::KW_ENDMODULE, id, line};
            if (id == "wire") return {Tok::KW_WIRE, id, line};
            if (id == "reg") return {Tok::KW_REG, id, line};
            if (id == "input") return {Tok::KW_INPUT, id, line};
            if (id == "output") return {Tok::KW_OUTPUT, id, line};
            if (id == "assign") return {Tok::KW_ASSIGN, id, line};
            return {Tok::IDENT, id, line};
        }

        if (std::isdigit((unsigned char)c)) {
            std::string num;
            while (pos < s.size() && (std::isalnum((unsigned char)peek()) || peek() == '\''))
                num += advance(); // grabs things like 4'b1010 too, kept as raw text for now
            return {Tok::NUMBER, num, line};
        }

        switch (c) {
            case '(': return makeSingle(Tok::LPAREN);
            case ')': return makeSingle(Tok::RPAREN);
            case '[': return makeSingle(Tok::LBRACK);
            case ']': return makeSingle(Tok::RBRACK);
            case ';': return makeSingle(Tok::SEMI);
            case ',': return makeSingle(Tok::COMMA);
            case '=': return makeSingle(Tok::EQUALS);
            case '+': return makeSingle(Tok::PLUS);
            case '-': return makeSingle(Tok::MINUS);
            case '&': return makeSingle(Tok::AMP);
            case '|': return makeSingle(Tok::PIPE);
            case '^': return makeSingle(Tok::CARET);
            case '~': return makeSingle(Tok::TILDE);
        }

        throw std::runtime_error("Lexer: unexpected character '" + std::string(1, c) +
                                  "' at line " + std::to_string(line));
    }
};
