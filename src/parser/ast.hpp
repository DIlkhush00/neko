#pragma once

#include "../lexer/token.hpp"
#include "visitor.hpp"

#include <vector>

struct Expr {
    virtual ~Expr() = default;
    virtual void accept(ExprVisitor&) = 0;
};

struct Stmt {
    virtual ~Stmt() = default;
    virtual void accept(StmtVisitor&) = 0;
};

struct UnaryExpr : Expr {
    Token op;
    Expr* right;

    UnaryExpr(Token operator_, Expr* rightExpr) : op(operator_), right(rightExpr) {}

    void accept(ExprVisitor& visitor) override { visitor.visitUnaryExpr(this); }
};

struct BinaryExpr : Expr {
    Expr* left;
    Token op;
    Expr* right;

    BinaryExpr(Expr* leftExpr, Token operator_, Expr* rightExpr)
        : left(leftExpr), op(operator_), right(rightExpr)
    {
    }

    void accept(ExprVisitor& visitor) override { visitor.visitBinaryExpr(this); }
};

struct LiteralExpr : Expr {
    Token value;

    LiteralExpr(Token val) : value(val) {}

    void accept(ExprVisitor& visitor) override { visitor.visitLiteralExpr(this); }
};

struct VariableExpr : Expr {
    Token name;

    VariableExpr(Token n) : name(n) {}

    void accept(ExprVisitor& visitor) override { visitor.visitVariableExpr(this); }
};

struct AssignmentExpr : Expr {
    Token name;
    Expr* value;

    AssignmentExpr(Token n, Expr* val) : name(n), value(val) {}

    void accept(ExprVisitor& visitor) override { visitor.visitAssignmentExpr(this); }
};

struct CallExpr : Expr {
    Expr* callee;
    std::vector<Expr*> arguments;
    Token paren; // closing parenthesis

    CallExpr(Expr* calleeExpr, std::vector<Expr*> args, Token closingParen)
        : callee(calleeExpr), arguments(std::move(args)), paren(closingParen)
    {
    }

    void accept(ExprVisitor& visitor) override { visitor.visitCallExpr(this); }
};

struct GroupingExpr : Expr {
    Expr* expression;

    GroupingExpr(Expr* expr) : expression(expr) {}

    void accept(ExprVisitor& visitor) override { visitor.visitGroupingExpr(this); }
};

struct ExpressionStmt : Stmt {
    Expr* expression;

    ExpressionStmt(Expr* expr) : expression(expr) {}

    void accept(StmtVisitor& visitor) override { visitor.visitExpressionStmt(this); }
};

struct PrintStmt : Stmt {
    Expr* expression;

    PrintStmt(Expr* expr) : expression(expr) {}

    void accept(StmtVisitor& visitor) override { visitor.visitPrintStmt(this); }
};

struct VarStmt : Stmt {
    Token name;
    Expr* initializer; // can be nullptr

    VarStmt(Token n, Expr* init) : name(n), initializer(init) {}

    void accept(StmtVisitor& visitor) override { visitor.visitVarStmt(this); }
};

struct IfStmt : Stmt {
    Expr* condition;
    Stmt* thenBranch;
    Stmt* elseBranch; // can be nullptr

    IfStmt(Expr* cond, Stmt* thenBr, Stmt* elseBr)
        : condition(cond), thenBranch(thenBr), elseBranch(elseBr)
    {
    }

    void accept(StmtVisitor& visitor) override { visitor.visitIfStmt(this); }
};

struct WhileStmt : Stmt {
    Expr* condition;
    Stmt* body;

    WhileStmt(Expr* cond, Stmt* bodyStmt) : condition(cond), body(bodyStmt) {}

    void accept(StmtVisitor& visitor) override { visitor.visitWhileStmt(this); }
};

struct FunctionStmt : Stmt {
    Token name;
    std::vector<Token> parameters;
    BlockStmt* body;

    FunctionStmt(Token n, std::vector<Token> params, BlockStmt* bodyStmts)
        : name(n), parameters(std::move(params)), body(bodyStmts)
    {
    }

    void accept(StmtVisitor& visitor) override { visitor.visitFunctionStmt(this); }
};

struct ReturnStmt : Stmt {
    Token keyword;
    Expr* value; // can be nullptr

    ReturnStmt(Token kw, Expr* val) : keyword(kw), value(val) {}

    void accept(StmtVisitor& visitor) override { visitor.visitReturnStmt(this); }
};

struct BlockStmt : Stmt {
    std::vector<Stmt*> statements;

    BlockStmt(std::vector<Stmt*> stmts) : statements(std::move(stmts)) {}

    void accept(StmtVisitor& visitor) override { visitor.visitBlockStmt(this); }
};
