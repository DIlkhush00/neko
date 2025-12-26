#include "codegen.hpp"

#include <algorithm>
#include <sstream>

namespace codegen {

std::string CodeGenerator::generate(const ir::Program& program)
{
    output.clear();
    variables.clear();
    string_literals.clear();
    next_string_id = 0;

    collect_variables(program);

    // Header
    output.push_back("section .note.GNU-stack noalloc noexec nowrite progbits");
    output.push_back("extern printf");
    output.push_back("extern exit");
    output.push_back("global main");
    output.push_back("");

    // Data section
    output.push_back("section .data");
    output.push_back("    fmt_int: db \"%ld\", 10, 0");
    output.push_back("    fmt_str: db \"%s\", 10, 0");
    for (auto const& [str, id] : string_literals)
    {
        output.push_back("    " + id + ": db `" + str + "`, 0");
    }
    output.push_back("");

    // BSS section
    output.push_back("section .bss");
    for (const auto& var : variables)
    {
        output.push_back("    " + var + ": resq 1");
    }
    output.push_back("");

    // Text section
    output.push_back("section .text");
    output.push_back("main:");
    output.push_back("    push rbp");
    output.push_back("    mov rbp, rsp");
    output.push_back("");

    const auto& instructions = program.get_instructions();
    for (const auto& inst : instructions)
    {
        switch (inst.op)
        {
        case ir::OpCode::ADD:
            emit("mov rax, " + map_operand(*inst.arg1));
            emit("add rax, " + map_operand(*inst.arg2));
            emit("mov " + map_operand(*inst.result) + ", rax");
            break;
        case ir::OpCode::SUB:
            emit("mov rax, " + map_operand(*inst.arg1));
            emit("sub rax, " + map_operand(*inst.arg2));
            emit("mov " + map_operand(*inst.result) + ", rax");
            break;
        case ir::OpCode::MUL:
            emit("mov rax, " + map_operand(*inst.arg1));
            emit("imul rax, " + map_operand(*inst.arg2));
            emit("mov " + map_operand(*inst.result) + ", rax");
            break;
        case ir::OpCode::DIV:
            emit("mov rax, " + map_operand(*inst.arg1));
            emit("cqo");
            emit("idiv qword " + map_operand(*inst.arg2));
            emit("mov " + map_operand(*inst.result) + ", rax");
            break;
        case ir::OpCode::ASSIGN:
            emit("mov rax, " + map_operand(*inst.arg1));
            emit("mov " + map_operand(*inst.result) + ", rax");
            break;
        case ir::OpCode::LABEL:
            emit_label(inst.arg1->name);
            break;
        case ir::OpCode::PROLOGUE:
            emit("push rbp");
            emit("mov rbp, rsp");
            break;
        case ir::OpCode::JUMP:
            emit("jmp " + inst.arg1->name);
            break;
        case ir::OpCode::JUMP_IF_FALSE:
            emit("mov rax, " + map_operand(*inst.arg1));
            emit("cmp rax, 0");
            emit("je " + inst.arg2->name);
            break;
        case ir::OpCode::JUMP_IF_TRUE:
            emit("mov rax, " + map_operand(*inst.arg1));
            emit("cmp rax, 0");
            emit("jne " + inst.arg2->name);
            break;
        case ir::OpCode::PRINT:
            if (inst.arg1->type == ir::OperandType::CONSTANT &&
                inst.arg1->value.front() == '"')
            {
                emit("mov rdi, fmt_str");
                emit("mov rsi, " + map_operand(*inst.arg1));
            }
            else
            {
                emit("mov rdi, fmt_int");
                emit("mov rsi, " + map_operand(*inst.arg1));
            }
            emit("xor rax, rax");
            emit("call printf");
            break;
        case ir::OpCode::RETURN:
            if (inst.arg1)
            {
                emit("mov rax, " + map_operand(*inst.arg1));
            }
            emit("mov rsp, rbp");
            emit("pop rbp");
            emit("ret");
            break;
        case ir::OpCode::HALT:
            emit("mov rdi, 0");
            emit("call exit");
            break;
        case ir::OpCode::LT:
            emit("mov rax, " + map_operand(*inst.arg1));
            emit("cmp rax, " + map_operand(*inst.arg2));
            emit("setl al");
            emit("movzx rax, al");
            emit("mov " + map_operand(*inst.result) + ", rax");
            break;
        case ir::OpCode::GT:
            emit("mov rax, " + map_operand(*inst.arg1));
            emit("cmp rax, " + map_operand(*inst.arg2));
            emit("setg al");
            emit("movzx rax, al");
            emit("mov " + map_operand(*inst.result) + ", rax");
            break;
        case ir::OpCode::LE:
            emit("mov rax, " + map_operand(*inst.arg1));
            emit("cmp rax, " + map_operand(*inst.arg2));
            emit("setle al");
            emit("movzx rax, al");
            emit("mov " + map_operand(*inst.result) + ", rax");
            break;
        case ir::OpCode::GE:
            emit("mov rax, " + map_operand(*inst.arg1));
            emit("cmp rax, " + map_operand(*inst.arg2));
            emit("setge al");
            emit("movzx rax, al");
            emit("mov " + map_operand(*inst.result) + ", rax");
            break;
        case ir::OpCode::EQ:
            emit("mov rax, " + map_operand(*inst.arg1));
            emit("cmp rax, " + map_operand(*inst.arg2));
            emit("sete al");
            emit("movzx rax, al");
            emit("mov " + map_operand(*inst.result) + ", rax");
            break;
        case ir::OpCode::NE:
            emit("mov rax, " + map_operand(*inst.arg1));
            emit("cmp rax, " + map_operand(*inst.arg2));
            emit("setne al");
            emit("movzx rax, al");
            emit("mov " + map_operand(*inst.result) + ", rax");
            break;
        case ir::OpCode::PARAM:
            emit("push qword " + map_operand(*inst.arg1));
            break;
        case ir::OpCode::CALL: {
            int num_args = std::stoi(inst.arg2->value);
            const char* reg_args[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
            for (int i = std::min(num_args, 6) - 1; i >= 0; --i)
            {
                emit("pop " + std::string(reg_args[i]));
            }
            emit("xor rax, rax");
            emit("call " + inst.arg1->name);
            if (num_args > 6)
            {
                emit("add rsp, " + std::to_string((num_args - 6) * 8));
            }
            emit("mov " + map_operand(*inst.result) + ", rax");
            break;
        }
        case ir::OpCode::PARAM_BIND: {
            int index = std::stoi(inst.arg2->value);
            const char* reg_args[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
            if (index < 6)
            {
                emit("mov [" + inst.arg1->name + "], " + reg_args[index]);
            }
            else
            {
                emit("mov rax, [rbp + " + std::to_string(16 + (index - 6) * 8) + "]");
                emit("mov [" + inst.arg1->name + "], rax");
            }
            break;
        }
        default:
            break;
        }
    }

    emit("mov rdi, 0");
    emit("call exit");

    std::stringstream ss;
    for (const auto& line : output)
    {
        ss << line << "\n";
    }
    return ss.str();
}

std::string CodeGenerator::map_operand(const ir::Operand& op)
{
    if (op.type == ir::OperandType::VARIABLE || op.type == ir::OperandType::TEMPORARY)
    {
        return "[" + op.name + "]";
    }
    else if (op.type == ir::OperandType::CONSTANT)
    {
        if (op.value.front() == '"')
        {
            return string_literals[op.value.substr(1, op.value.length() - 2)];
        }
        if (op.value == "true")
            return "1";
        if (op.value == "false")
            return "0";
        if (op.value == "null")
            return "0";
        return op.value;
    }
    return op.name;
}

void CodeGenerator::collect_variables(const ir::Program& program)
{
    std::unordered_set<std::string> labels;
    for (const auto& inst : program.get_instructions())
    {
        if (inst.op == ir::OpCode::LABEL)
        {
            labels.insert(inst.arg1->name);
        }
    }

    for (const auto& inst : program.get_instructions())
    {
        if (inst.result)
        {
            if (inst.result->type == ir::OperandType::VARIABLE ||
                inst.result->type == ir::OperandType::TEMPORARY)
            {
                if (labels.find(inst.result->name) == labels.end())
                {
                    variables.insert(inst.result->name);
                }
            }
        }
        if (inst.arg1)
        {
            if (inst.arg1->type == ir::OperandType::VARIABLE ||
                inst.arg1->type == ir::OperandType::TEMPORARY)
            {
                if (labels.find(inst.arg1->name) == labels.end())
                {
                    variables.insert(inst.arg1->name);
                }
            }
            else if (inst.arg1->type == ir::OperandType::CONSTANT &&
                     inst.arg1->value.front() == '"')
            {
                std::string s =
                    inst.arg1->value.substr(1, inst.arg1->value.length() - 2);
                if (string_literals.find(s) == string_literals.end())
                {
                    string_literals[s] = "str_" + std::to_string(next_string_id++);
                }
            }
        }
        if (inst.arg2)
        {
            if (inst.arg2->type == ir::OperandType::VARIABLE ||
                inst.arg2->type == ir::OperandType::TEMPORARY)
            {
                if (labels.find(inst.arg2->name) == labels.end())
                {
                    variables.insert(inst.arg2->name);
                }
            }
        }
    }
}

} // namespace codegen
