#pragma once

#include "../parser/ast.hpp"
#include "../parser/visitor.hpp"
#include "sema_error.hpp"
#include "symbol_table.hpp"

#include <vector>

class SemanticAnalyzer : public ExprVisitor, public StmtVisitor {
  public:
    SemanticAnalyzer(ErrorHandler& errorHandler) : error_handler(errorHandler) {}

    void analyze(const std::vector<Stmt*>& statements);

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
    ErrorHandler& error_handler;
    SymbolTable symbol_table;

    enum class FunctionType { NONE, FUNCTION };

    FunctionType current_function = FunctionType::NONE;

    void resolve(Stmt* stmt);
    void resolve(Expr* expr);
    void resolve_function(FunctionStmt* function, FunctionType type);
};
