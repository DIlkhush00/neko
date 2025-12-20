#pragma once

#include <optional>
#include <string>

enum class TokenType {
    // single-character tokens
    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_BRACE,
    RIGHT_BRACE,
    COMMA,
    DOT,
    SEMICOLON,
    PLUS,
    MINUS,
    STAR,
    SLASH,
    BANG,
    EQUAL,
    LESS,
    GREATER,

    // one or two character tokens
    BANG_EQUAL,
    EQUAL_EQUAL,
    LESS_EQUAL,
    GREATER_EQUAL,

    // literals
    IDENTIFIER,
    STRING,
    NUMBER,

    // keywords
    VAR,
    FUNCTION,
    RETURN,
    PRINT,
    WHILE,
    IF,
    ELSE,
    TRUE,
    FALSE,
    NULL_TOK,

    // special tokens
    EOF_TOK,
    INVALID
};

struct Token {
    TokenType type;
    std::optional<std::string> value;
    unsigned long line;
    unsigned long column;
};
