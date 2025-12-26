#pragma once

#include "../parser/ast.hpp"
#include "../parser/visitor.hpp"
#include "tac.hpp"

#include <string>
#include <vector>

namespace ir {

class IRGenerator : public ExprVisitor, public StmtVisitor {
  public:
    IRGenerator() = default;

    Program generate(const std::vector<Stmt*>& statements);

    void visitBinaryExpr(BinaryExpr* expr) override;
    void visitUnaryExpr(UnaryExpr* expr) override;
    void visitLiteralExpr(LiteralExpr* expr) override;
    void visitVariableExpr(VariableExpr* expr) override;
    void visitAssignmentExpr(AssignmentExpr* expr) override;
    void visitCallExpr(CallExpr* expr) override;
    void visitGroupingExpr(GroupingExpr* expr) override;

    void visitExpressionStmt(ExpressionStmt* stmt) override;
    void visitPrintStmt(PrintStmt* stmt) override;
    void visitBlockStmt(BlockStmt* stmt) override;
    void visitIfStmt(IfStmt* stmt) override;
    void visitWhileStmt(WhileStmt* stmt) override;
    void visitReturnStmt(ReturnStmt* stmt) override;
    void visitVarStmt(VarStmt* stmt) override;
    void visitFunctionStmt(FunctionStmt* stmt) override;

  private:
    Program program;
    int next_temp = 0;
    int next_label = 0;
    Operand last_expr_result = Operand::constant("null");

    Operand new_temp() { return Operand::temporary(next_temp++); }
    Operand new_label(const std::string& prefix = "L")
    {
        return Operand::label(prefix + std::to_string(next_label++));
    }

    void emit(OpCode op,
              std::optional<Operand> res = std::nullopt,
              std::optional<Operand> a1 = std::nullopt,
              std::optional<Operand> a2 = std::nullopt)
    {
        program.add_instruction({op, res, a1, a2});
    }

    void gen(Stmt* stmt) { stmt->accept(*this); }
    Operand gen(Expr* expr)
    {
        expr->accept(*this);
        return last_expr_result;
    }
};

} // namespace ir
