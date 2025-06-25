#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <vector>
#include "ast_printer.hpp"
#include "semantic_codegen.hpp"


int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cerr << "Incorrect usage. Correct usage is..." << std::endl;
        std::cerr << "hydro <input.hy>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string contents;
    {
        std::stringstream contents_stream;
        std::fstream input(argv[1], std::ios::in);
        contents_stream << input.rdbuf();
        contents = contents_stream.str();
    }



     Tokenizer tokenizer(std::move(contents));
     std::vector<Token> tokens = tokenizer.tokenize();

     std::cout << tokens << std::endl;

     Parser parser(std::move(tokens));
     std::optional<NodeProg> prog = parser.parse_prog();

     if (!prog.has_value()) {
         std::cerr << "Invalid program" << std::endl;
         exit(EXIT_FAILURE);
     }
    // SemanticAnalyzer analyzer(prog.value());
    // analyzer.analyze();

    std::cout << "Generated AST:\n";
    std::cout << prog.value() << std::endl;

     {
         Generator generator(prog.value());
         std::fstream file("out.asm", std::ios::out);
         file << generator.gen_prog();
        file.close();
     }

    std::cout << "\nGenerated Assembly Code\n";
    std::ifstream assembly_file("out.asm");
    std::string line;
    if (assembly_file.is_open()) {
        while (getline(assembly_file, line)) {
            std::cout << line << std::endl;
        }
        assembly_file.close();
    } else {
        std::cerr << "Error opening out.asm for reading." << std::endl;
    }

     system("nasm -felf64 out.asm");
     system("ld -o out out.o");
     int status = system("./out");

     int exitCode = WEXITSTATUS(status);
     std::cout << "Output is : " << exitCode << std::endl;



    return EXIT_SUCCESS;
}
