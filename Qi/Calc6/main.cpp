/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include "ast.hpp"
#include "calculator.hpp"
#include "compiler.hpp"
#include "error_handler.hpp"
#include "vm.hpp"

int main() {
    using namespace client;

    using iterator_type = std::string::const_iterator;
    using calculator = calculator<iterator_type>;
    using ast::expression;

    std::string str{"1.1 * 2.2 - (3.3 + 4.4) + -1.1"};
    double expected = 1.1 * 2.2 - (3.3 + 4.4) + -1.1;

    vmachine vm;               // Our virtual machine
    std::vector<double> code;  // Our VM code
    calculator calc;           // Our grammar
    expression expr;           // Our expression (AST)
    compiler compile(code);    // Compiles the expression

    auto iter = str.cbegin();
    auto end = str.cend();
    boost::spirit::ascii::space_type space;
    bool success = phrase_parse(iter, end, calc, space, expr);

    if (success && iter == end) {
        std::cout << "-------------------------\n";
        std::cout << "Parsing succeeded\n";
        compile(expr);
        vm.execute(code);
        std::cout << "\nResult: " << vm.top();
        std::cout << "\nExpected: " << expected << std::endl;
        std::cout << "-------------------------\n";
    } else {
        std::string rest(iter, end);
        std::cout << "-------------------------\n";
        std::cout << "Parsing failed\n";
        std::cout << "-------------------------\n";
        std::cout << "Stopped at: \"" << rest << "\"\n";
    }

    return 0;
}