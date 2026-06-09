#include "calculator.hpp"
#include <iostream>
#include <string>

int main() {
    auto& calc = client::calculator;

    boost::spirit::x3::ascii::space_type space;

    std::string str{"1.1 * 2.2 - (3.3 + 4.4) + -1.1"};

    auto iter = str.begin();
    auto end = str.end();
    bool r = phrase_parse(iter, end, calc, space);

    if (r && iter == end) {
        std::cout << "-------------------------\n";
        std::cout << "Parsing succeeded\n";
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