#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <filesystem>
#include <vector>
#include <optional>

enum class TokenType
{
    RETURN,
    NUMBER,
    SEMICOLON,
    END_OF_FILE
};

struct Token
{
    TokenType type;
    std::optional<std::string> value;
};


std::vector<Token> tokenize(const std::string& source)
{
    std::vector<Token> tokens;
    size_t pos = 0;
    while(pos < source.length())
    {
        if(source.compare(pos, 6, "return") == 0)
        {
            tokens.push_back({TokenType::RETURN, std::nullopt});
            pos += 6;
        }
        else if(std::isdigit(source[pos]))
        {
            size_t start = pos;
            while(pos < source.length() && std::isdigit(source[pos]))
                pos++;
            tokens.push_back({TokenType::NUMBER, source.substr(start, pos - start)});
        }
        else if(source[pos] == ';')
        {
            tokens.push_back({TokenType::SEMICOLON, std::nullopt});
            pos++;
        }
        else if(std::isspace(source[pos]))
        {
            pos++;
        }
        else
        {
            std::cerr << "Unexpected character: " << source[pos] << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    tokens.push_back({TokenType::END_OF_FILE, std::nullopt});
    return tokens;
}

std::vector<std::string> parse(const std::vector<Token>& tokens)
{
    std::vector<std::string> ast;

    // basic setup in ast
    const std::string prologue = 
        "global _start\n"
        "_start:\n";
    ast.push_back(prologue);

    size_t pos = 0;
    while(pos < tokens.size())
    {
        if(tokens[pos].type == TokenType::RETURN)
        {
            pos++;
            if(tokens[pos].type != TokenType::NUMBER)
            {
                std::cerr << "Error: Expected number after return" << std::endl;
                exit(EXIT_FAILURE);
            }
            std::string returnValue = tokens[pos].value.value();
            pos++;
            if(tokens[pos].type != TokenType::SEMICOLON)
            {
                std::cerr << "Error: Expected semicolon after return value" << std::endl;
                exit(EXIT_FAILURE);
            }
            pos++;
            // generate assembly for return statement
            ast.push_back("    mov rax, 60\n");
            ast.push_back("    mov rdi, " + returnValue + "\n");
            ast.push_back("    syscall\n");
        }
        else if(tokens[pos].type == TokenType::END_OF_FILE)
        {
            break;
        }
        else
        {
            std::cerr << "Error: Unexpected token" << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    return ast;
}


int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        std::cout << "Usage: neko <source-file>.js" << std::endl;
        return EXIT_FAILURE;
    }

    // change working directory to build/
    try 
    {
        std::filesystem::current_path("build");
    } 
    catch(const std::exception& e) 
    {
        std::cerr << "Error: Could not switch to build/ directory: "
                << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    
    /*
        steps:
            1. read the test.js
            2. break it into tokens
            3. parse the tokens
            4. generate the assembly code
            5. output the assembly code to a file
    */

    // read the test.js file
    std::cout  << "Reading source file: " << argv[1] << std::endl;
    std::cout << std::endl;
    std::filesystem::path sourcePath = argv[1];
    std::string sourceCode;
    {
        std::ifstream sourceFile(sourcePath, std::ios::in);
        if(!sourceFile.is_open())
        {
            std::cerr << "Error: Could not open file " << argv[1] << std::endl;
            return EXIT_FAILURE;
        }

        std::stringstream buffer;
        buffer << sourceFile.rdbuf();
        sourceCode = buffer.str();
    }

    // tokenize the source code
    std::cout << "Tokenizing source code..." << std::endl;
    std::cout << std::endl;
    std::vector<Token> tokens = tokenize(sourceCode);

    // parse the tokens
    std::cout << "Parsing tokens..." << std::endl;
    std::cout << std::endl;
    std::vector<std::string> ast = parse(tokens);

    // generate assembly code
    std::cout << "Generating assembly code..." << std::endl;
    std::cout << std::endl;
    std::string assemblyCode;
    for(const auto& line : ast)
    {
        assemblyCode += line;
    }

    // output the assembly code to a file
    std::filesystem::path assemblyPath = sourcePath.stem();
    assemblyPath += ".asm";
    std::cout << "Writing assembly code to file: " << assemblyPath << std::endl;
    std::cout << std::endl;
    {
        std::ofstream assemblyFile(assemblyPath, std::ios::out);
        if(!assemblyFile.is_open())
        {
            std::cerr << "Error: Could not open file " << assemblyPath << " for writing" << std::endl;
            return EXIT_FAILURE;
        }
        assemblyFile << assemblyCode;
    }

    // execute nasm and ld to generate executable
    std::cout << "Assembling and linking..." << std::endl;
    std::cout << std::endl;
    system(("nasm -f elf64 " + assemblyPath.string() + " -o " + sourcePath.stem().string() + ".o").c_str());
    system(("ld " + sourcePath.stem().string() + ".o -o " + sourcePath.stem().string()).c_str());

    // run the executable
    std::cout << "Running the executable..." << std::endl;
    system(("./" + sourcePath.stem().string() + " ; echo $? ").c_str());

    return EXIT_SUCCESS;
}