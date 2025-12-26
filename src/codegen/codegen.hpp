#pragma once

#include "../ir/tac.hpp"

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace codegen {

class CodeGenerator {
  public:
    std::string generate(const ir::Program& program);

  private:
    std::vector<std::string> output;
    std::unordered_set<std::string> variables;
    std::unordered_map<std::string, std::string> string_literals;
    int next_string_id = 0;

    void emit(const std::string& instr) { output.push_back("    " + instr); }

    void emit_label(const std::string& label) { output.push_back(label + ":"); }

    std::string map_operand(const ir::Operand& op);
    void collect_variables(const ir::Program& program);
};

} // namespace codegen
