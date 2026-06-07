/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include "compiler.hpp"
#include "statement.hpp"
#include "vm.hpp"

int main() {
    using iterator_type = std::string::const_iterator;
    using namespace client;

    double a = 123.3;
    double b = 456.1;
    double c = a + b * 2;
    double d = c - a / 3;
    double e = -a;

    std::string source{
        "var a = 123.3;\n"
        "var b = 456.1;\n"
        "var c = a + b * 2;\n"
        "var d = c - a / 3;\n"
        "var e = -a;"};

    auto iter = source.cbegin();
    auto end = source.cend();

    vmachine vm;                // Our virtual machine
    code_gen::program program;  // Our VM program
    ast::statement_list ast;    // Our AST

    error_handler<iterator_type> error_handler(iter, end);  // Our error handler
    parser::statement<iterator_type> parser(error_handler);  // Our parser
    code_gen::compiler compile(program, error_handler);      // Our compiler

    boost::spirit::ascii::space_type space;

    if (bool success = phrase_parse(iter, end, parser, space, ast);
        success && iter == end) {
        if (compile(ast)) {
            std::cout << "Success\n";
            std::cout << "-------------------------\n";
            vm.execute(program.code());

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
            std::cout << "    d: " << d << "\n";
            std::cout << "    e: " << e << "\n";
        } else {
            std::cout << "Compile failure\n";
        }
    } else {
        std::cout << "Parse failure\n";
    }

    return 0;
}