#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <filesystem>
#include <vector>

#include "lexer/lexer.hpp"

int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        std::cout << "Usage: neko <source-file>.js" << std::endl;
        return EXIT_FAILURE;
    }

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

    std::cout  << "Reading source file: " << argv[1] << std::endl;
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
    
    while (!lexer.is_at_end()) 
    {
        tokens.push_back(lexer.next_token());
    }

    for (const auto& token : tokens) 
    {
        if (token.value.has_value()) 
        {
            std::cout << "Token: " << lexer.tokenToString(token.type) << ", Value: " << token.value.value() << std::endl;
        } 
        else 
        {
            std::cout << "Token: " << lexer.tokenToString(token.type) << std::endl;
        }
    }
    std::cout << std::endl;

    return EXIT_SUCCESS;
}