#include "parser.hpp"

#include <iostream>

std::vector<Stmt*> Parser::parse()
{
    std::vector<Stmt*> statements;
    while (!is_at_end())
    {
        Stmt* decl = nullptr;
        try
        {
            decl = declaration();
        } catch (const ParseError& err)
        {
            std::cerr << "Parse error: " << err.what() << "\n";
            synchronize();
            continue;
        }

        if (decl != nullptr)
            statements.push_back(decl);
    }
    return statements;
}

Stmt* Parser::declaration()
{
    try
    {
        if (match({TokenType::VAR}))
            return var_declaration();
        if (match({TokenType::FUNCTION}))
            return function_declaration();

        return statement();
    } catch (const ParseError&)
    {
        synchronize();
        return nullptr;
    }
}

Stmt* Parser::statement()
{
    if (match({TokenType::RETURN}))
        return return_statement();
    if (match({TokenType::PRINT}))
        return print_statement();
    if (match({TokenType::IF}))
        return if_statement();
    if (match({TokenType::WHILE}))
        return while_statement();
    if (match({TokenType::LEFT_BRACE}))
        return block();

    return expression_statement();
}

Stmt* Parser::var_declaration()
{
    Token name = consume(TokenType::IDENTIFIER, "Expect variable name after 'var'.");
    consume(TokenType::EQUAL, "Expect '=' after variable name.");
    Expr* initializer = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
    return new VarStmt{name, initializer};
}

Stmt* Parser::function_declaration()
{
    Token name =
        consume(TokenType::IDENTIFIER, "Expect function name after 'function'.");
    consume(TokenType::LEFT_PAREN, "Expect '(' after function name.");

    std::vector<Token> params;
    if (!check(TokenType::RIGHT_PAREN))
    {
        do
        {
            Token param = consume(TokenType::IDENTIFIER, "Expect parameter name.");
            params.push_back(param);
        } while (match({TokenType::COMMA}));
    }

    consume(TokenType::RIGHT_PAREN, "Expect ')' after parameters.");
    consume(TokenType::LEFT_BRACE, "Expect '{' before function body.");
    BlockStmt* body = block();

    return new FunctionStmt{name, params, body};
}

Stmt* Parser::return_statement()
{
    Token keyword = previous();
    Expr* value = nullptr;
    if (!check(TokenType::SEMICOLON))
    {
        value = expression();
    }
    consume(TokenType::SEMICOLON, "Expect ';' after return value.");
    return new ReturnStmt{keyword, value};
}

Stmt* Parser::print_statement()
{
    Expr* value = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after value.");
    return new PrintStmt{value};
}

Stmt* Parser::if_statement()
{
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'if'.");
    Expr* condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after if condition.");

    consume(TokenType::LEFT_BRACE, "Expect '{' before if body.");
    BlockStmt* thenBranch = block();

    Stmt* elseBranch = nullptr;
    if (match({TokenType::ELSE}))
    {
        consume(TokenType::LEFT_BRACE, "Expect '{' before else body.");
        elseBranch = block();
    }

    return new IfStmt{condition, thenBranch, elseBranch};
}

Stmt* Parser::while_statement()
{
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'while'.");
    Expr* condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after condition.");

    consume(TokenType::LEFT_BRACE, "Expect '{' before while body.");
    BlockStmt* body = block();

    return new WhileStmt{condition, body};
}

Stmt* Parser::expression_statement()
{
    Expr* expr = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after expression.");
    return new ExpressionStmt{expr};
}

BlockStmt* Parser::block()
{
    std::vector<Stmt*> statements;

    while (!check(TokenType::RIGHT_BRACE) && !is_at_end())
    {
        Stmt* decl = declaration();
        if (decl != nullptr)
            statements.push_back(decl);
    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");
    return new BlockStmt{statements};
}

Expr* Parser::expression() { return assignment(); }

Expr* Parser::assignment()
{
    Expr* expr = equality();

    if (match({TokenType::EQUAL}))
    {
        Token equals = previous();
        Expr* value = assignment();

        if (auto varExpr = dynamic_cast<VariableExpr*>(expr))
        {
            Token name = varExpr->name;
            return new AssignmentExpr{name, value};
        }

        error(equals, "Invalid assignment target.");
        throw ParseError("Invalid assignment target.");
    }

    return expr;
}

Expr* Parser::equality()
{
    Expr* expr = comparison();

    while (match({TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL}))
    {
        Token op = previous();
        Expr* right = comparison();
        expr = new BinaryExpr{expr, op, right};
    }

    return expr;
}

Expr* Parser::comparison()
{
    Expr* expr = term();

    while (match({TokenType::GREATER,
                  TokenType::GREATER_EQUAL,
                  TokenType::LESS,
                  TokenType::LESS_EQUAL}))
    {
        Token op = previous();
        Expr* right = term();
        expr = new BinaryExpr{expr, op, right};
    }

    return expr;
}

Expr* Parser::term()
{
    Expr* expr = factor();

    while (match({TokenType::PLUS, TokenType::MINUS}))
    {
        Token op = previous();
        Expr* right = factor();
        expr = new BinaryExpr{expr, op, right};
    }

    return expr;
}

Expr* Parser::factor()
{
    Expr* expr = unary();

    while (match({TokenType::STAR, TokenType::SLASH}))
    {
        Token op = previous();
        Expr* right = unary();
        expr = new BinaryExpr{expr, op, right};
    }

    return expr;
}

Expr* Parser::unary()
{
    if (match({TokenType::BANG, TokenType::MINUS}))
    {
        Token op = previous();
        Expr* right = unary();
        return new UnaryExpr{op, right};
    }

    return call();
}

Expr* Parser::call()
{
    Expr* expr = primary();

    while (true)
    {
        if (match({TokenType::LEFT_PAREN}))
        {
            expr = finishCall(expr);
        }
        else
        {
            break;
        }
    }

    return expr;
}

Expr* Parser::finishCall(Expr* callee)
{
    std::vector<Expr*> arguments;
    if (!check(TokenType::RIGHT_PAREN))
    {
        do
        {
            arguments.push_back(expression());
            if (arguments.size() > 255)
            {
                error(peek(), "Can't have more than 255 arguments.");
                throw ParseError("Too many function arguments.");
            }
        } while (match({TokenType::COMMA}));
    }

    Token paren = consume(TokenType::RIGHT_PAREN, "Expect ')' after arguments.");

    return new CallExpr{callee, arguments, paren};
}

Expr* Parser::primary()
{
    if (match({TokenType::FALSE}))
        return new LiteralExpr{previous()};
    if (match({TokenType::TRUE}))
        return new LiteralExpr{previous()};
    if (match({TokenType::NULL_TOK}))
        return new LiteralExpr{previous()};

    if (match({TokenType::NUMBER, TokenType::STRING}))
    {
        return new LiteralExpr{previous()};
    }

    if (match({TokenType::IDENTIFIER}))
    {
        return new VariableExpr{previous()};
    }

    if (match({TokenType::LEFT_PAREN}))
    {
        Expr* expr = expression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
        return new GroupingExpr{expr};
    }

    // No match found
    error(peek(), "Expect expression.");
    throw ParseError("Expect expression.");
}

Token Parser::consume(TokenType type, const std::string& message)
{
    if (check(type))
        return advance();
    error(peek(), message);
    throw ParseError(message);
}

void Parser::synchronize()
{
    advance();

    while (!is_at_end())
    {
        if (previous().type == TokenType::SEMICOLON)
            return;

        switch (peek().type)
        {
        case TokenType::FUNCTION:
        case TokenType::VAR:
        case TokenType::IF:
        case TokenType::WHILE:
        case TokenType::PRINT:
        case TokenType::RETURN:
            return;
        default:
            break;
        }

        advance();
    }
}

void Parser::error(const Token& token, const std::string& message)
{
    std::cerr << "[line " << token.line << "] Error at ";
    if (token.type == TokenType::EOF_TOK)
    {
        std::cerr << "end";
    }
    else
    {
        std::cerr << "'" << (token.value.has_value() ? token.value.value() : "") << "'";
    }
    std::cerr << ": " << message << "\n";
}

bool Parser::match(const std::vector<TokenType>& types)
{
    for (const auto& type : types)
    {
        if (check(type))
        {
            advance();
            return true;
        }
    }
    return false;
}

bool Parser::check(TokenType type) const
{
    if (is_at_end())
        return false;
    return peek().type == type;
}

Token Parser::advance()
{
    if (!is_at_end())
        m_current++;
    return previous();
}

bool Parser::is_at_end() const { return peek().type == TokenType::EOF_TOK; }

Token Parser::peek() const { return m_tokens[m_current]; }

Token Parser::previous() const
{
    if (m_current == 0)
        return Token{};
    return m_tokens[m_current - 1];
}
