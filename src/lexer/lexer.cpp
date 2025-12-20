#include "lexer.hpp"

const std::unordered_map<std::string, TokenType> Lexer::keywords = {
    {"var", TokenType::VAR},
    {"function", TokenType::FUNCTION},
    {"return", TokenType::RETURN},
    {"print", TokenType::PRINT},
    {"while", TokenType::WHILE},
    {"if", TokenType::IF},
    {"else", TokenType::ELSE},
    {"true", TokenType::TRUE},
    {"false", TokenType::FALSE},
    {"null", TokenType::NULL_TOK}};

Lexer::Lexer(const std::string& source)
    : m_src(source), m_index(0), m_line(1), m_column(1)
{
}

Token Lexer::next_token()
{
    skip_whitespace();

    if (is_at_end())
    {
        return make_token(TokenType::EOF_TOK);
    }

    char c = advance();
    switch (c)
    {
    case '(':
        return make_token(TokenType::LEFT_PAREN);
    case ')':
        return make_token(TokenType::RIGHT_PAREN);
    case '{':
        return make_token(TokenType::LEFT_BRACE);
    case '}':
        return make_token(TokenType::RIGHT_BRACE);
    case ',':
        return make_token(TokenType::COMMA);
    case '.':
        return make_token(TokenType::DOT);
    case ';':
        return make_token(TokenType::SEMICOLON);
    case '+':
        return make_token(TokenType::PLUS);
    case '-':
        return make_token(TokenType::MINUS);
    case '*':
        return make_token(TokenType::STAR);
    case '/':
        return make_token(TokenType::SLASH);
    case '!':
        return make_token(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
    case '=':
        return make_token(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
    case '<':
        return make_token(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
    case '>':
        return make_token(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
    case '"':
        return string_literal();
    default:
        if (std::isdigit(c))
        {
            return number();
        }
        else if (std::isalpha(c) || c == '_')
        {
            return identifier();
        }
        else
        {
            return make_token(TokenType::INVALID);
        }
    }
}

Token Lexer::peek_token()
{
    size_t saved_m_index = m_index;
    int saved_m_line = m_line;
    int saved_m_column = m_column;

    Token token = next_token();

    m_index = saved_m_index;
    m_line = saved_m_line;
    m_column = saved_m_column;

    return token;
}

bool Lexer::is_at_end() const { return m_index >= m_src.length(); }

char Lexer::advance()
{
    char c = m_src[m_index++];
    if (c == '\n')
    {
        m_line++;
        m_column = 1;
    }
    else
    {
        m_column++;
    }

    return c;
}

char Lexer::peek() const
{
    if (is_at_end())
        return '\0';
    return m_src[m_index];
}

char Lexer::peek_next() const
{
    if (m_index + 1 >= m_src.length())
        return '\0';
    return m_src[m_index + 1];
}

bool Lexer::match(char expected)
{
    if (is_at_end())
        return false;
    if (m_src[m_index] != expected)
        return false;
    m_index++;
    m_column++;
    return true;
}

Token Lexer::identifier()
{
    size_t start = m_index - 1;

    while (std::isalnum(peek()) || peek() == '_')
    {
        advance();
    }

    std::string text = m_src.substr(start, m_index - start);

    auto it = keywords.find(text);
    TokenType type = (it != keywords.end()) ? it->second : TokenType::IDENTIFIER;

    return {type, text, m_line, m_column - (m_index - start)};
}

Token Lexer::number()
{
    size_t start = m_index - 1;
    while (std::isdigit(peek()))
    {
        advance();
    }

    std::string text = m_src.substr(start, m_index - start);

    return {TokenType::NUMBER, text, m_line, m_column - (m_index - start)};
}

Token Lexer::string_literal()
{
    size_t start = m_index;
    while (peek() != '"' && !is_at_end())
    {
        advance();
    }

    if (is_at_end())
    {
        return make_token(TokenType::INVALID);
    }

    advance(); // closing "

    std::string text = m_src.substr(start, m_index - start - 1);

    return {TokenType::STRING, text, m_line, m_column - (m_index - start + 1)};
}

Token Lexer::make_token(TokenType type)
{
    return {type, std::nullopt, m_line, m_column - 1};
}

void Lexer::skip_whitespace()
{
    while (true)
    {
        char c = peek();
        switch (c)
        {
        case ' ':
        case '\r':
        case '\t':
            advance();
            break;
        case '\n':
            advance();
            break;
        case '/':
            if (peek_next() == '/')
            {
                skip_comment();
            }
            else
            {
                return;
            }
            break;
        default:
            return;
        }
    }
}

void Lexer::skip_comment()
{
    while (peek() != '\n' && !is_at_end())
    {
        advance();
    }
}

const std::string& Lexer::tokenToString(TokenType type) const
{
    static const std::string tokenStrings[] = {
        "LEFT_PAREN", "RIGHT_PAREN", "LEFT_BRACE", "RIGHT_BRACE",   "COMMA",
        "DOT",        "SEMICOLON",   "PLUS",       "MINUS",         "STAR",
        "SLASH",      "BANG",        "EQUAL",      "LESS",          "GREATER",

        "BANG_EQUAL", "EQUAL_EQUAL", "LESS_EQUAL", "GREATER_EQUAL",

        "IDENTIFIER", "STRING",      "NUMBER",

        "VAR",        "FUNCTION",    "RETURN",     "PRINT",         "WHILE",
        "IF",         "ELSE",        "TRUE",       "FALSE",         "NULL_TOK",

        "EOF_TOK",    "INVALID"};

    return tokenStrings[static_cast<int>(type)];
}