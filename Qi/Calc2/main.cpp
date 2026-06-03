/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include "calculator.hpp"
#include <string>

int main() {
    using iterator_type = std::string::const_iterator;
    using calculator = client::calculator<iterator_type>;

    boost::spirit::ascii::space_type space;  // Our skipper
    calculator calc;                         // Our grammar

    std::string str{"1.1 * 2.2 - (3.3 + 4.4) + -1.1"};

    auto iter = str.cbegin();
    auto end = str.cend();
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