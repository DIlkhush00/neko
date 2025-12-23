#include "semantic_analyzer.hpp"

void SemanticAnalyzer::analyze(const std::vector<Stmt*>& statements)
{
    symbol_table.enter_scope(); // global scope
    for (Stmt* statement : statements)
    {
        resolve(statement);
    }
}

void SemanticAnalyzer::resolve(Stmt* stmt) { stmt->accept(*this); }

void SemanticAnalyzer::resolve(Expr* expr) { expr->accept(*this); }

void SemanticAnalyzer::visitBinaryExpr(BinaryExpr* expr)
{
    resolve(expr->left);
    resolve(expr->right);
}

void SemanticAnalyzer::visitUnaryExpr(UnaryExpr* expr) { resolve(expr->right); }

void SemanticAnalyzer::visitLiteralExpr(LiteralExpr* expr) {}

void SemanticAnalyzer::visitVariableExpr(VariableExpr* expr)
{
    std::string name = expr->name.value.value();
    if (!symbol_table.resolve(name))
    {
        error_handler.report(expr->name, "Undefined variable '" + name + "'.");
    }
}

void SemanticAnalyzer::visitAssignmentExpr(AssignmentExpr* expr)
{
    resolve(expr->value);
    std::string name = expr->name.value.value();
    if (!symbol_table.resolve(name))
    {
        error_handler.report(expr->name, "Undefined variable '" + name + "'.");
    }
}

void SemanticAnalyzer::visitCallExpr(CallExpr* expr)
{
    resolve(expr->callee);
    for (Expr* argument : expr->arguments)
    {
        resolve(argument);
    }
}

void SemanticAnalyzer::visitGroupingExpr(GroupingExpr* expr)
{
    resolve(expr->expression);
}

void SemanticAnalyzer::visitExpressionStmt(ExpressionStmt* stmt)
{
    resolve(stmt->expression);
}

void SemanticAnalyzer::visitPrintStmt(PrintStmt* stmt) { resolve(stmt->expression); }

void SemanticAnalyzer::visitBlockStmt(BlockStmt* stmt)
{
    symbol_table.enter_scope(); // new scope
    for (Stmt* statement : stmt->statements)
    {
        resolve(statement);
    }
    symbol_table.exit_scope();
}

void SemanticAnalyzer::visitIfStmt(IfStmt* stmt)
{
    resolve(stmt->condition);
    resolve(stmt->thenBranch);
    if (stmt->elseBranch != nullptr)
    {
        resolve(stmt->elseBranch);
    }
}

void SemanticAnalyzer::visitWhileStmt(WhileStmt* stmt)
{
    resolve(stmt->condition);
    resolve(stmt->body);
}

void SemanticAnalyzer::visitReturnStmt(ReturnStmt* stmt)
{
    // or current_function == FunctionType::NONE both are fine
    if (symbol_table.is_at_global_scope())
    {
        error_handler.report(stmt->keyword,
                             "Invalid return statement outside of a function.");
    }

    if (stmt->value != nullptr)
    {
        resolve(stmt->value);
    }
}

void SemanticAnalyzer::visitVarStmt(VarStmt* stmt)
{
    if (stmt->initializer != nullptr)
    {
        resolve(stmt->initializer);
    }

    std::string name = stmt->name.value.value();
    if (!symbol_table.define(name, {name, SymbolType::VARIABLE}))
    {
        error_handler.report(stmt->name,
                             "Identifier '" + name +
                                 "' is already defined in the current scope.");
    }
}

void SemanticAnalyzer::visitFunctionStmt(FunctionStmt* stmt)
{
    std::string name = stmt->name.value.value();
    if (!symbol_table.define(name, {name, SymbolType::FUNCTION}))
    {
        error_handler.report(stmt->name,
                             "Identifier '" + name +
                                 "' is already defined in the current scope.");
    }

    resolve_function(stmt, FunctionType::FUNCTION);
}

void SemanticAnalyzer::resolve_function(FunctionStmt* function, FunctionType type)
{
    FunctionType enclosing_function = current_function;
    current_function = type;

    symbol_table.enter_scope();
    for (const Token& param : function->parameters)
    {
        std::string param_name = param.value.value();
        if (!symbol_table.define(param_name, {param_name, SymbolType::VARIABLE}))
        {
            error_handler.report(param,
                                 "Identifier '" + param_name +
                                     "' is already defined in the current scope.");
        }
    }

    for (Stmt* statement : function->body->statements)
    {
        resolve(statement);
    }
    symbol_table.exit_scope();

    current_function = enclosing_function;
}
