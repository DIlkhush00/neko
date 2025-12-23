#pragma once

#include "../lexer/token.hpp"

#include <iostream>
#include <string>
#include <vector>

struct SemanticError {
    Token token;
    std::string message;

    SemanticError(Token t, std::string msg) : token(t), message(std::move(msg)) {}
};

class ErrorHandler {
  public:
    void report(Token token, const std::string& message)
    {
        errors.push_back({token, message});
        std::cerr << "[Line " << token.line << ":" << token.column
                  << "] Semantic Error: " << message << std::endl;
    }

    bool has_errors() const { return !errors.empty(); }
    const std::vector<SemanticError>& get_errors() const { return errors; }

  private:
    std::vector<SemanticError> errors;
};
