/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#pragma once

// Spirit v2.5 allows you to suppress automatic generation
// of predefined terminals to speed up complation. With
// BOOST_SPIRIT_NO_PREDEFINED_TERMINALS defined, you are
// responsible in creating instances of the terminals that
// you need (e.g. see qi::uint_type uint_ below).
#ifndef BOOST_SPIRIT_NO_PREDEFINED_TERMINALS
#define BOOST_SPIRIT_NO_PREDEFINED_TERMINALS
#endif

#include <boost/spirit/include/qi.hpp>
#include <iostream>

namespace client {
namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

template <typename IT>
class calculator : public qi::grammar<IT, ascii::space_type> {
   public:
    calculator() : calculator::base_type(expression_) {
        qi::double_type double_;
        qi::char_type char_;

        expression_ = term_ >> *((char_('+') >> term_[do_add]) |
                                 (char_('-') >> term_[do_subt]));

        term_ = factor_ >> *((char_('*') >> factor_[do_mult]) |
                             (char_('/') >> factor_[do_div]));

        factor_ = double_[do_double] | char_('(') >> expression_ >> char_(')');
    }

   private:
    qi::rule<IT, ascii::space_type> expression_;
    qi::rule<IT, ascii::space_type> term_;
    qi::rule<IT, ascii::space_type> factor_;

    static inline auto do_double = [](double n) {
        std::cout << "push " << n << std::endl;
    };
    static inline auto do_add = []() { std::cout << "add\n"; };
    static inline auto do_subt = []() { std::cout << "subtract\n"; };
    static inline auto do_mult = []() { std::cout << "mult\n"; };
    static inline auto do_div = []() { std::cout << "divide\n"; };
};
}  // namespace client