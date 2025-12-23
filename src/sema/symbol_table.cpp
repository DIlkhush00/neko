#include "symbol_table.hpp"

void SymbolTable::enter_scope() { scopes.push_back({}); }

void SymbolTable::exit_scope()
{
    if (!scopes.empty())
    {
        scopes.pop_back();
    }
}

bool SymbolTable::define(const std::string& name, Symbol symbol)
{
    if (scopes.empty())
        return false;

    // check if already defined in current scope
    if (scopes.back().find(name) != scopes.back().end())
    {
        return false;
    }

    scopes.back()[name] = symbol;
    return true;
}

std::optional<Symbol> SymbolTable::resolve(const std::string& name)
{
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it)
    {
        auto found = it->find(name);
        if (found != it->end())
        {
            return found->second;
        }
    }
    return std::nullopt;
}

bool SymbolTable::is_at_global_scope() const { return scopes.size() == 1; }