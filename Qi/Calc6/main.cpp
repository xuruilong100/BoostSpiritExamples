#include "ast.hpp"
#include "calculator.hpp"
#include "compiler.hpp"
#include "error_handler.hpp"
#include "vm.hpp"

int main() {
    using iterator_type = std::string::const_iterator;
    using calculator = client::calculator<iterator_type>;
    using ast_expression = client::ast::expression;
    using compiler = client::compiler;
    using vmachine = client::vmachine;

    std::string str{"1.1 * 2.2 - (3.3 + 4.4) + -1.1"};
    double expected = 1.1 * 2.2 - (3.3 + 4.4) + -1.1;

    vmachine vm;                // Our virtual machine
    std::vector<double> code;   // Our VM code
    calculator calc;            // Our grammar
    ast_expression expression;  // Our program (AST)
    compiler compile(code);     // Compiles the program

    auto iter = str.cbegin();
    auto end = str.cend();
    boost::spirit::ascii::space_type space;
    bool r = phrase_parse(iter, end, calc, space, expression);

    if (r && iter == end) {
        std::cout << "-------------------------\n";
        std::cout << "Parsing succeeded\n";
        compile(expression);
        vm.execute(code);
        std::cout << "\nResult: " << vm.top();
        std::cout << "\nExpected: " << expected << std::endl;
        std::cout << "-------------------------\n";
    } else {
        std::string rest(iter, end);
        std::cout << "-------------------------\n";
        std::cout << "Parsing failed\n";
        std::cout << "-------------------------\n";
    }

    return 0;
}