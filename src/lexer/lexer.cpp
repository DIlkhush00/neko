#include "lexer.hpp"

Lexer::Lexer(const std::string& source)
    : src(source), index(0), line(1), column(1) {}

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
        case '(': return make_token(TokenType::LEFT_PAREN);
        case ')': return make_token(TokenType::RIGHT_PAREN);
        case '{': return make_token(TokenType::LEFT_BRACE);
        case '}': return make_token(TokenType::RIGHT_BRACE);
        case ',': return make_token(TokenType::COMMA);
        case '.': return make_token(TokenType::DOT);
        case ';': return make_token(TokenType::SEMICOLON);
        case '+': return make_token(TokenType::PLUS);
        case '-': return make_token(TokenType::MINUS);
        case '*': return make_token(TokenType::STAR);
        case '/': return make_token(TokenType::SLASH);
        case '!':
            return make_token(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
        case '=':
            return make_token(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
        case '<':
            return make_token(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
        case '>':
            return make_token(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
        case '"': return string_literal();
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
    size_t saved_index = index;
    int saved_line = line;
    int saved_column = column;

    Token token = next_token();

    index = saved_index;
    line = saved_line;
    column = saved_column;

    return token;
}

bool Lexer::is_at_end() const 
{
    return index >= src.length();
}

char Lexer::advance() 
{
    char c = src[index++];
    if (c == '\n') 
    {
        line++;
        column = 1;
    } 
    else 
    {
        column++;
    }

    return c;
}

char Lexer::peek() const 
{
    if (is_at_end()) return '\0';
    return src[index];
}

char Lexer::peek_next() const 
{
    if (index + 1 >= src.length()) return '\0';
    return src[index + 1];
}

bool Lexer::match(char expected) 
{
    if (is_at_end()) return false;
    if (src[index] != expected) return false;
    index++;
    column++;
    return true;
}

Token Lexer::identifier() 
{
    size_t start = index - 1;

    while (std::isalnum(peek()) || peek() == '_') 
    {
        advance();
    }

    std::string text = src.substr(start, index - start);
    TokenType type = TokenType::IDENTIFIER;

    if (text == "var") type = TokenType::VAR;
    else if (text == "function") type = TokenType::FUNCTION;
    else if (text == "return") type = TokenType::RETURN;
    else if (text == "for") type = TokenType::FOR;
    else if (text == "while") type = TokenType::WHILE;
    else if (text == "if") type = TokenType::IF;
    else if (text == "else") type = TokenType::ELSE;
    else if (text == "true") type = TokenType::TRUE;
    else if (text == "false") type = TokenType::FALSE;
    else if (text == "null") type = TokenType::NULL_TOK;

    return {type, text, line, column - (index - start)};
}

Token Lexer::number() 
{
    size_t start = index - 1;
    while (std::isdigit(peek())) 
    {
        advance();
    }

    std::string text = src.substr(start, index - start);

    return {TokenType::NUMBER, text, line, column - (index - start)};
}

Token Lexer::string_literal() 
{
    size_t start = index;
    while (peek() != '"' && !is_at_end()) 
    {
        advance();
    }

    if (is_at_end()) 
    {
        return make_token(TokenType::INVALID);
    }

    advance(); // closing "

    std::string text = src.substr(start, index - start - 1);

    return {TokenType::STRING, text, line, column - (index - start + 1)};
}

Token Lexer::make_token(TokenType type) 
{
    return {type, std::nullopt, line, column - 1};
}

void Lexer::skip_whitespace() 
{
    while (true) 
    {
        char c = peek();
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
                advance();
                break;
            case '\n':
                advance();
                break;
            case '/':
                if (peek_next() == '/') {
                    skip_comment();
                } else {
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
    static const std::string tokenStrings[] = 
    {
        "LEFT_PAREN", "RIGHT_PAREN",
        "LEFT_BRACE", "RIGHT_BRACE",
        "COMMA", "DOT", "SEMICOLON",
        "PLUS", "MINUS", "STAR", "SLASH",
        "BANG", "EQUAL",
        "LESS", "GREATER",

        "BANG_EQUAL",
        "EQUAL_EQUAL",
        "LESS_EQUAL",
        "GREATER_EQUAL",

        "IDENTIFIER", "STRING", "NUMBER",

        "VAR", "FUNCTION", "RETURN",
        "FOR", "WHILE",
        "IF", "ELSE",
        "TRUE", "FALSE",
        "NULL_TOK",
        
        "EOF_TOK",
        "INVALID"
    };

    return tokenStrings[static_cast<int>(type)];
}