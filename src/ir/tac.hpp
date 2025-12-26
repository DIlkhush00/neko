#pragma once

#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace ir {

enum class OperandType { VARIABLE, TEMPORARY, CONSTANT, LABEL };

struct Operand {
    OperandType type;
    std::string name;
    std::string value;

    static Operand variable(std::string name)
    {
        return {OperandType::VARIABLE, std::move(name), ""};
    }
    static Operand temporary(int id)
    {
        return {OperandType::TEMPORARY, "t" + std::to_string(id), ""};
    }
    static Operand constant(std::string value)
    {
        return {OperandType::CONSTANT, "", std::move(value)};
    }
    static Operand label(std::string name)
    {
        return {OperandType::LABEL, std::move(name), ""};
    }

    std::string to_string() const
    {
        if (type == OperandType::CONSTANT)
            return value;
        return name;
    }
};

enum class OpCode {
    ADD,
    SUB,
    MUL,
    DIV,
    NOT,
    ASSIGN,
    JUMP,
    JUMP_IF_FALSE,
    JUMP_IF_TRUE,
    LABEL,
    CALL,
    RETURN,
    PARAM,
    PARAM_BIND,
    PRINT,
    HALT,
    PROLOGUE,
    LT,
    GT,
    LE,
    GE,
    EQ,
    NE
};

struct Instruction {
    OpCode op;
    std::optional<Operand> result;
    std::optional<Operand> arg1;
    std::optional<Operand> arg2;

    std::string to_string() const
    {
        switch (op)
        {
        case OpCode::ADD:
            return result->to_string() + " = " + arg1->to_string() + " + " +
                   arg2->to_string();
        case OpCode::SUB:
            return result->to_string() + " = " + arg1->to_string() + " - " +
                   arg2->to_string();
        case OpCode::MUL:
            return result->to_string() + " = " + arg1->to_string() + " * " +
                   arg2->to_string();
        case OpCode::DIV:
            return result->to_string() + " = " + arg1->to_string() + " / " +
                   arg2->to_string();
        case OpCode::NOT:
            return result->to_string() + " = !" + arg1->to_string();
        case OpCode::ASSIGN:
            return result->to_string() + " = " + arg1->to_string();
        case OpCode::JUMP:
            return "goto " + arg1->to_string();
        case OpCode::JUMP_IF_FALSE:
            return "ifFalse " + arg1->to_string() + " goto " + arg2->to_string();
        case OpCode::JUMP_IF_TRUE:
            return "ifTrue " + arg1->to_string() + " goto " + arg2->to_string();
        case OpCode::LABEL:
            return arg1->to_string() + ":";
        case OpCode::CALL:
            return (result ? result->to_string() + " = " : "") + "call " +
                   arg1->to_string() + ", " + arg2->to_string();
        case OpCode::RETURN:
            return "return " + (arg1 ? arg1->to_string() : "");
        case OpCode::PARAM:
            return "param " + arg1->to_string();
        case OpCode::PARAM_BIND:
            return "bind_param " + arg1->to_string() + ", " + arg2->to_string();
        case OpCode::PRINT:
            return "print " + arg1->to_string();
        case OpCode::HALT:
            return "halt";
        case OpCode::PROLOGUE:
            return "prologue";
        case OpCode::LT:
            return result->to_string() + " = " + arg1->to_string() + " < " +
                   arg2->to_string();
        case OpCode::GT:
            return result->to_string() + " = " + arg1->to_string() + " > " +
                   arg2->to_string();
        case OpCode::LE:
            return result->to_string() + " = " + arg1->to_string() +
                   " <= " + arg2->to_string();
        case OpCode::GE:
            return result->to_string() + " = " + arg1->to_string() +
                   " >= " + arg2->to_string();
        case OpCode::EQ:
            return result->to_string() + " = " + arg1->to_string() +
                   " == " + arg2->to_string();
        case OpCode::NE:
            return result->to_string() + " = " + arg1->to_string() +
                   " != " + arg2->to_string();
        default:
            return "unknown";
        }
    }
};

class Program {
  public:
    void add_instruction(Instruction inst) { instructions.push_back(std::move(inst)); }
    const std::vector<Instruction>& get_instructions() const { return instructions; }

    const Instruction* get_last_instruction() const
    {
        if (instructions.empty())
            return nullptr;
        return &instructions.back();
    }

    void print() const
    {
        for (const auto& inst : instructions)
        {
            if (inst.op == OpCode::LABEL)
            {
                std::cout << inst.to_string() << std::endl;
            }
            else
            {
                std::cout << "  " << inst.to_string() << std::endl;
            }
        }
    }

  private:
    std::vector<Instruction> instructions;
};

} // namespace ir
