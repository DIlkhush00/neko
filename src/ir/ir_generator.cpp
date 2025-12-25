#include "ir_generator.hpp"

#include <iostream>

namespace ir {

Program IRGenerator::generate(const std::vector<Stmt*>& statements)
{
    std::vector<Stmt*> functions;
    std::vector<Stmt*> globals;

    for (Stmt* stmt : statements)
    {
        if (dynamic_cast<FunctionStmt*>(stmt))
        {
            functions.push_back(stmt);
        }
        else
        {
            globals.push_back(stmt);
        }
    }

    // emit globals first
    for (Stmt* stmt : globals)
    {
        gen(stmt);
    }

    // Halt after globals to prevent falling into functions - this one is important!!
    if (!functions.empty())
    {
        emit(OpCode::HALT);
    }

    // emit functions
    for (Stmt* stmt : functions)
    {
        gen(stmt);
    }

    return std::move(program);
}

void IRGenerator::visitBinaryExpr(BinaryExpr* expr)
{
    Operand left = gen(expr->left);
    Operand right = gen(expr->right);
    Operand result = new_temp();

    OpCode op;
    switch (expr->op.type)
    {
    case TokenType::PLUS:
        op = OpCode::ADD;
        break;
    case TokenType::MINUS:
        op = OpCode::SUB;
        break;
    case TokenType::STAR:
        op = OpCode::MUL;
        break;
    case TokenType::SLASH:
        op = OpCode::DIV;
        break;
    case TokenType::LESS:
        op = OpCode::LT;
        break;
    case TokenType::GREATER:
        op = OpCode::GT;
        break;
    case TokenType::LESS_EQUAL:
        op = OpCode::LE;
        break;
    case TokenType::GREATER_EQUAL:
        op = OpCode::GE;
        break;
    case TokenType::EQUAL_EQUAL:
        op = OpCode::EQ;
        break;
    case TokenType::BANG_EQUAL:
        op = OpCode::NE;
        break;
    default:
        return; // should not happen after sema
    }

    emit(op, result, left, right);
    last_expr_result = result;
}

void IRGenerator::visitUnaryExpr(UnaryExpr* expr)
{
    Operand right = gen(expr->right);
    Operand result = new_temp();

    OpCode op;
    if (expr->op.type == TokenType::BANG)
        op = OpCode::NOT;
    else
        return;

    emit(op, result, right);
    last_expr_result = result;
}

void IRGenerator::visitLiteralExpr(LiteralExpr* expr)
{
    std::string val =
        expr->value.value.has_value() ? expr->value.value.value() : "null";

    if (expr->value.type == TokenType::TRUE)
        val = "true";
    else if (expr->value.type == TokenType::FALSE)
        val = "false";
    else if (expr->value.type == TokenType::STRING)
        val = "\"" + val + "\"";

    last_expr_result = Operand::constant(val);
}

void IRGenerator::visitVariableExpr(VariableExpr* expr)
{
    last_expr_result = Operand::variable(expr->name.value.value());
}

void IRGenerator::visitAssignmentExpr(AssignmentExpr* expr)
{
    Operand value = gen(expr->value);
    emit(OpCode::ASSIGN, Operand::variable(expr->name.value.value()), value);
    last_expr_result = Operand::variable(expr->name.value.value());
}

void IRGenerator::visitCallExpr(CallExpr* expr)
{
    std::vector<Operand> args;
    for (Expr* arg : expr->arguments)
    {
        args.push_back(gen(arg));
    }

    for (const auto& arg : args)
    {
        emit(OpCode::PARAM, std::nullopt, arg);
    }

    Operand callee = gen(expr->callee);
    Operand result = new_temp();
    emit(OpCode::CALL, result, callee, Operand::constant(std::to_string(args.size())));
    last_expr_result = result;
}

void IRGenerator::visitGroupingExpr(GroupingExpr* expr)
{
    last_expr_result = gen(expr->expression);
}

void IRGenerator::visitExpressionStmt(ExpressionStmt* stmt) { gen(stmt->expression); }

void IRGenerator::visitPrintStmt(PrintStmt* stmt)
{
    Operand value = gen(stmt->expression);
    emit(OpCode::PRINT, std::nullopt, value);
}

void IRGenerator::visitBlockStmt(BlockStmt* stmt)
{
    for (Stmt* s : stmt->statements)
    {
        gen(s);
    }
}

void IRGenerator::visitIfStmt(IfStmt* stmt)
{
    Operand condition = gen(stmt->condition);
    Operand else_label = new_label("else");
    Operand end_label = new_label("endif");

    emit(OpCode::JUMP_IF_FALSE, std::nullopt, condition, else_label);
    gen(stmt->thenBranch);
    emit(OpCode::JUMP, std::nullopt, end_label);

    emit(OpCode::LABEL, std::nullopt, else_label);
    if (stmt->elseBranch)
    {
        gen(stmt->elseBranch);
    }
    emit(OpCode::LABEL, std::nullopt, end_label);
}

void IRGenerator::visitWhileStmt(WhileStmt* stmt)
{
    Operand start_label = new_label("while_start");
    Operand end_label = new_label("while_end");

    emit(OpCode::LABEL, std::nullopt, start_label);
    Operand condition = gen(stmt->condition);
    emit(OpCode::JUMP_IF_FALSE, std::nullopt, condition, end_label);

    gen(stmt->body);
    emit(OpCode::JUMP, std::nullopt, start_label);

    emit(OpCode::LABEL, std::nullopt, end_label);
}

void IRGenerator::visitReturnStmt(ReturnStmt* stmt)
{
    if (stmt->value)
    {
        Operand value = gen(stmt->value);
        emit(OpCode::RETURN, std::nullopt, value);
    }
    else
    {
        emit(OpCode::RETURN);
    }
}

void IRGenerator::visitVarStmt(VarStmt* stmt)
{
    if (stmt->initializer)
    {
        Operand value = gen(stmt->initializer);
        emit(OpCode::ASSIGN, Operand::variable(stmt->name.value.value()), value);
    }
}

void IRGenerator::visitFunctionStmt(FunctionStmt* stmt)
{
    emit(OpCode::LABEL, std::nullopt, Operand::label(stmt->name.value.value()));

    for (size_t i = 0; i < stmt->parameters.size(); ++i)
    {
        emit(OpCode::PARAM_BIND,
             std::nullopt,
             Operand::variable(stmt->parameters[i].value.value()),
             Operand::constant(std::to_string(i)));
    }

    gen(stmt->body);

    // only emit implicit return if the last instruction wasn't already a return
    const Instruction* last = program.get_last_instruction();
    if (!last || last->op != OpCode::RETURN)
    {
        emit(OpCode::RETURN);
    }
}

}
