/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include "ast.hpp"
#include "calculator.hpp"

int main() {
    using namespace client;
    using iterator_type = std::string::const_iterator;
    using calculator = calculator<iterator_type>;
    using ast_program = ast::program;
    using ast_print = ast::printer;
    using ast_eval = ast::eval;

    std::string str{"1.1 * 2.2 - (3.3 + 4.4) + -1.1"};
    double expected = 1.1 * 2.2 - (3.3 + 4.4) + -1.1;

    calculator calc;      // Our grammar
    ast_program program;  // Our program (AST)
    ast_print print;      // Prints the program
    ast_eval eval;        // Evaluates the program

    auto iter = str.cbegin();
    auto end = str.cend();
    boost::spirit::ascii::space_type space;
    bool r = phrase_parse(iter, end, calc, space, program);

    if (r && iter == end) {
        std::cout << "-------------------------\n";
        std::cout << "Parsing succeeded\n";
        print(program);
        std::cout << "\nResult: " << eval(program);
        std::cout << "\nExpected: " << expected << std::endl;
        std::cout << "-------------------------\n";
    } else {
        std::string rest(iter, end);
        std::cout << "-------------------------\n";
        std::cout << "Parsing failed\n";
        std::cout << "stopped at: \" " << rest << "\"\n";
        std::cout << "-------------------------\n";
    }

    return 0;
}