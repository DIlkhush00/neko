#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

enum class SymbolType { VARIABLE, FUNCTION };

struct Symbol {
    std::string name;
    SymbolType type;
};

class SymbolTable {
  public:
    void enter_scope();
    void exit_scope();

    bool define(const std::string& name, Symbol symbol);
    std::optional<Symbol> resolve(const std::string& name);

    bool is_at_global_scope() const;

  private:
    std::vector<std::unordered_map<std::string, Symbol>> scopes;
};
