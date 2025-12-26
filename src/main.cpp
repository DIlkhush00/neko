#include "codegen/codegen.hpp"
#include "ir/ir_generator.hpp"
#include "lexer/lexer.hpp"
#include "parser/ast_printer.hpp"
#include "parser/parser.hpp"
#include "sema/semantic_analyzer.hpp"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage: neko <source-file>.js" << std::endl;
        return EXIT_FAILURE;
    }

    try
    {
        std::filesystem::current_path("build");
    } catch (const std::exception& e)
    {
        std::cerr << "Error: Could not switch to build/ directory: " << e.what()
                  << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Reading source file: " << argv[1] << std::endl;
    std::cout << std::endl;

    std::filesystem::path sourcePath = argv[1];
    std::string sourceCode;

    {
        std::ifstream sourceFile(sourcePath, std::ios::in);
        if (!sourceFile.is_open())
        {
            std::cerr << "Error: Could not open file " << argv[1] << std::endl;
            return EXIT_FAILURE;
        }

        std::stringstream buffer;
        buffer << sourceFile.rdbuf();
        sourceCode = buffer.str();
    }

    std::cout << "Tokenizing source code..." << std::endl;
    std::cout << std::endl;

    Lexer lexer(sourceCode);
    std::vector<Token> tokens;

    Token token;
    do
    {
        token = lexer.next_token();
        tokens.push_back(token);
    } while (token.type != TokenType::EOF_TOK);

    std::cout << "Parsing tokens into AST..." << std::endl;
    std::cout << std::endl;

    Parser parser(tokens);
    std::vector<Stmt*> statements = parser.parse();

    std::cout << "Performing semantic analysis..." << std::endl;
    std::cout << std::endl;

    ErrorHandler semaErrorHandler;
    SemanticAnalyzer analyzer(semaErrorHandler);
    analyzer.analyze(statements);

    if (semaErrorHandler.has_errors())
    {
        std::cerr << "Semantic analysis failed with "
                  << semaErrorHandler.get_errors().size() << " errors." << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Printing AST..." << std::endl;
    std::cout << std::endl;

    AstPrinter printer;
    printer.print(statements);

    std::cout << std::endl;
    std::cout << "Generating Intermediate Representation (3AC - Three-Address Code)..."
              << std::endl;
    std::cout << std::endl;

    ir::IRGenerator ir_gen;
    ir::Program ir_program = ir_gen.generate(statements);

    std::cout << "Instructions:" << std::endl;
    ir_program.print();

    std::cout << std::endl;
    std::cout << "Generating Target Code (Assembly)..." << std::endl;
    std::cout << std::endl;

    codegen::CodeGenerator code_gen;
    std::string assembly = code_gen.generate(ir_program);

    std::cout << "Target Assembly:" << std::endl;
    std::cout << assembly << std::endl;

    std::ofstream outFile("output.asm");
    if (outFile.is_open())
    {
        outFile << assembly;
        outFile.close();
        std::cout << "Assembly saved to output.asm" << std::endl;
    }
    else
    {
        std::cerr << "Error: Could not write to output.asm" << std::endl;
    }

    return EXIT_SUCCESS;
}