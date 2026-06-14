#include "compiler.hpp"
#include "statement.hpp"
#include "vm.hpp"

int main() {
    using iterator_type = std::string::const_iterator;
    using namespace client;

    double a = 1.3;
    double b = 2.4;
    if (a >= 1.2) {
        a = b;
    } else {
        a = 20.5;
    }
    while (b < 10.5) {
        b = b + 1.1;
    }
    double c = 4.3;
    if (a >= 1.2) {
        a = b;
    } else {
        a = 20.5;
    }
    std::string source{
        "var a = 1.3;"
        "var b = 2.4;"
        "if (a >= 1.2) {"
        "   a = b;"
        "} "
        "else {"
        "   a = 20.5;"
        "}"
        "while (b < 10.5) {"
        "   b = b + 1.1;"
        "}"
        "var c = 4.3;"
        "if (a >= 1.2) {"
        "   a = b;"
        "} "
        "else {"
        "   a = 20.5;"
        "}"};

    auto iter = source.cbegin();
    auto end = source.cend();

    vmachine vm;                // Our virtual machine
    code_gen::program program;  // Our VM program
    ast::statement_list ast;    // Our AST

    error_handler<iterator_type> error_handler(iter, end);
    parser::statement<iterator_type> parser(error_handler);
    code_gen::compiler compile(program, error_handler);

    boost::spirit::ascii::space_type space;
    bool success = phrase_parse(iter, end, parser, space, ast);

    if (success && iter == end) {
        if (compile.start(ast)) {
            std::cout << "Success\n";
            std::cout << "-------------------------\n";
            vm.execute(program.code(), program.ptr(), program.value());

            std::cout << "-------------------------\n";
            std::cout << "Assembler----------------\n\n";
            program.print_assembler();

            std::cout << "-------------------------\n";
            std::cout << "Results------------------\n\n";
            program.print_variables(vm.get_stack());
            std::cout << "Expected results:\n";
            std::cout << "    a: " << a << "\n";
            std::cout << "    b: " << b << "\n";
            std::cout << "    c: " << c << "\n";
        } else {
            std::cout << "Compile failure\n";
        }
    } else {
        std::cout << "Parse failure\n";
    }

    std::cout << "-------------------------\n\n";
    return 0;
}