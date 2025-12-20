#include "../lexer/lexer.hpp"
#include "ast.hpp"

#include <iostream>

class AstPrinter : public ExprVisitor, public StmtVisitor {
    int indent = 0;

    void printIndent()
    {
        for (int i = 0; i < indent; i++)
            std::cout << "  ";
    }

  public:
    const std::string& tokenToString(TokenType type) const
    {
        static const std::string tokenStrings[] = {
            "LEFT_PAREN", "RIGHT_PAREN", "LEFT_BRACE", "RIGHT_BRACE",   "COMMA",
            "DOT",        "SEMICOLON",   "PLUS",       "MINUS",         "STAR",
            "SLASH",      "BANG",        "EQUAL",      "LESS",          "GREATER",

            "BANG_EQUAL", "EQUAL_EQUAL", "LESS_EQUAL", "GREATER_EQUAL",

            "IDENTIFIER", "STRING",      "NUMBER",

            "VAR",        "FUNCTION",    "RETURN",     "PRINT",         "WHILE",
            "IF",         "ELSE",        "TRUE",       "FALSE",         "NULL_TOK",

            "EOF_TOK",    "INVALID"};

        return tokenStrings[static_cast<int>(type)];
    }

    void print(const std::vector<Stmt*>& program)
    {
        for (Stmt* stmt : program)
        {
            stmt->accept(*this);
        }
    }

    void visitBlockStmt(BlockStmt* stmt) override
    {
        printIndent();
        std::cout << "Block\n";
        indent++;
        for (auto s : stmt->statements)
            s->accept(*this);
        indent--;
    }

    void visitIfStmt(IfStmt* stmt) override
    {
        printIndent();
        std::cout << "If\n";
        indent++;

        printIndent();
        std::cout << "Condition:\n";
        indent++;
        stmt->condition->accept(*this);
        indent--;

        printIndent();
        std::cout << "Then:\n";
        indent++;
        stmt->thenBranch->accept(*this);
        indent--;

        if (stmt->elseBranch)
        {
            printIndent();
            std::cout << "Else:\n";
            indent++;
            stmt->elseBranch->accept(*this);
            indent--;
        }

        indent--;
    }

    void visitWhileStmt(WhileStmt* stmt) override
    {
        printIndent();
        std::cout << "While\n";
        indent++;

        printIndent();
        std::cout << "Condition:\n";
        indent++;
        stmt->condition->accept(*this);
        indent--;

        printIndent();
        std::cout << "Body:\n";
        indent++;
        stmt->body->accept(*this);
        indent--;

        indent--;
    }

    void visitReturnStmt(ReturnStmt* stmt) override
    {
        printIndent();
        std::cout << "Return\n";
        indent++;
        if (stmt->value)
        {
            stmt->value->accept(*this);
        }
        indent--;
    }

    void visitFunctionStmt(FunctionStmt* stmt) override
    {
        printIndent();
        std::cout << "Function " << stmt->name.value.value() << "\n";
        indent++;

        printIndent();
        std::cout << "Parameters:\n";
        indent++;
        for (const auto& param : stmt->parameters)
        {
            printIndent();
            std::cout << param.value.value() << "\n";
        }
        indent--;

        printIndent();
        std::cout << "Body:\n";
        indent++;
        stmt->body->accept(*this);
        indent--;

        indent--;
    }

    void visitExpressionStmt(ExpressionStmt* stmt) override
    {
        printIndent();
        std::cout << "ExprStmt\n";
        indent++;
        stmt->expression->accept(*this);
        indent--;
    }

    void visitPrintStmt(PrintStmt* stmt) override
    {
        printIndent();
        std::cout << "Print\n";
        indent++;
        stmt->expression->accept(*this);
        indent--;
    }

    void visitVarStmt(VarStmt* stmt) override
    {
        // std::cout << "visit var is where things went wrong\n";
        printIndent();
        std::cout << "Var " << stmt->name.value.value() << "\n";
        indent++;
        if (stmt->initializer)
        {
            printIndent();
            std::cout << "Initializer:\n";
            indent++;
            stmt->initializer->accept(*this);
            indent--;
        }
        indent--;
    }

    void visitBinaryExpr(BinaryExpr* expr) override
    {
        // std::cout << "binary var is where things went wrong\n";
        printIndent();
        std::cout << "Binary (" << tokenToString(expr->op.type) << ")\n";
        indent++;
        expr->left->accept(*this);
        expr->right->accept(*this);
        indent--;
    }

    void visitUnaryExpr(UnaryExpr* expr) override
    {
        // std::cout << "unary var is where things went wrong\n";
        printIndent();
        std::cout << "Unary (" << expr->op.value.value() << ")\n";
        indent++;
        expr->right->accept(*this);
        indent--;
    }

    void visitLiteralExpr(LiteralExpr* expr) override
    {
        // std::cout << "literal var is where things went wrong\n";
        printIndent();
        std::cout << "Literal " << expr->value.value.value() << "\n";
    }

    void visitVariableExpr(VariableExpr* expr) override
    {
        // std::cout << "variable var is where things went wrong\n";
        printIndent();
        std::cout << "Variable " << expr->name.value.value() << "\n";
    }

    void visitAssignmentExpr(AssignmentExpr* expr) override
    {
        // std::cout << "assignment var is where things went wrong\n";
        printIndent();
        std::cout << "Assign " << expr->name.value.value() << "\n";
        indent++;
        expr->value->accept(*this);
        indent--;
    }

    void visitCallExpr(CallExpr* expr) override
    {
        printIndent();
        std::cout << "Call\n";
        indent++;

        printIndent();
        std::cout << "Callee:\n";
        indent++;
        expr->callee->accept(*this);
        indent--;

        printIndent();
        std::cout << "Args:\n";
        indent++;
        for (auto arg : expr->arguments)
            arg->accept(*this);
        indent--;

        indent--;
    }

    void visitGroupingExpr(GroupingExpr* expr) override
    {
        printIndent();
        std::cout << "Group\n";
        indent++;
        expr->expression->accept(*this);
        indent--;
    }
};
