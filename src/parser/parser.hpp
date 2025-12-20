#pragma once

#include "../lexer/token.hpp"
#include "ast.hpp"
#include "parse_error.hpp"

#include <vector>

class Parser {
  public:
    Parser(const std::vector<Token>& tokens) : m_tokens(tokens), m_current(0) {}
    std::vector<Stmt*> parse();

  private:
    Stmt* declaration();
    Stmt* statement();

    Stmt* var_declaration();
    Stmt* function_declaration();
    Stmt* return_statement();
    Stmt* print_statement();
    Stmt* if_statement();
    Stmt* while_statement();
    Stmt* expression_statement();
    BlockStmt* block();

    Expr* expression();
    Expr* assignment();
    Expr* equality();
    Expr* comparison();
    Expr* term();
    Expr* factor();
    Expr* unary();
    Expr* call();
    Expr* finishCall(Expr* callee);
    Expr* primary();

    Token consume(TokenType type, const std::string& message);
    void synchronize();
    void error(const Token& token, const std::string& message);

    bool match(const std::vector<TokenType>& types);
    bool check(TokenType type) const;
    Token advance();
    bool is_at_end() const;
    Token peek() const;
    Token previous() const;

    const std::vector<Token>& m_tokens;
    size_t m_current;
};