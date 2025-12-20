#pragma once

#include "token.hpp"

#include <string>
#include <unordered_map>

class Lexer {
  public:
    Lexer(const std::string& source);

    Token next_token();
    Token peek_token();
    bool is_at_end() const;
    const std::string& tokenToString(TokenType type) const; // for debugging

  private:
    char advance();
    char peek() const;
    char peek_next() const;

    bool match(char expected);

    Token identifier();
    Token number();
    Token string_literal();
    Token make_token(TokenType type);

    void skip_whitespace();
    void skip_comment();

    const std::string& m_src;
    size_t m_index;
    unsigned long m_line;
    unsigned long m_column;

    static const std::unordered_map<std::string, TokenType> keywords;
};
