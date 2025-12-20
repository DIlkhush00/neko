#pragma once

#include <stdexcept>
#include <string>

class ParseError : public std::runtime_error {
  public:
    ParseError(const std::string& msg) : std::runtime_error(msg) {}
};