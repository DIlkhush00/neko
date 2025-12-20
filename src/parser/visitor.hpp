#pragma once

struct BinaryExpr;
struct UnaryExpr;
struct LiteralExpr;
struct VariableExpr;
struct AssignmentExpr;
struct CallExpr;
struct GroupingExpr;

struct ExpressionStmt;
struct BlockStmt;
struct IfStmt;
struct WhileStmt;
struct ReturnStmt;
struct VarStmt;
struct FunctionStmt;
struct PrintStmt;

struct ExprVisitor {

    virtual void visitBinaryExpr(BinaryExpr*) = 0;
    virtual void visitUnaryExpr(UnaryExpr*) = 0;
    virtual void visitLiteralExpr(LiteralExpr*) = 0;
    virtual void visitVariableExpr(VariableExpr*) = 0;
    virtual void visitAssignmentExpr(AssignmentExpr*) = 0;
    virtual void visitCallExpr(CallExpr*) = 0;
    virtual void visitGroupingExpr(GroupingExpr*) = 0;
};

struct StmtVisitor {
    virtual void visitExpressionStmt(ExpressionStmt*) = 0;
    virtual void visitPrintStmt(PrintStmt*) = 0;
    virtual void visitBlockStmt(BlockStmt*) = 0;
    virtual void visitIfStmt(IfStmt*) = 0;
    virtual void visitWhileStmt(WhileStmt*) = 0;
    virtual void visitReturnStmt(ReturnStmt*) = 0;
    virtual void visitVarStmt(VarStmt*) = 0;
    virtual void visitFunctionStmt(FunctionStmt*) = 0;
};
