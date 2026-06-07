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

#include "ast.hpp"
#include "error_handler.hpp"
#include <boost/phoenix/function.hpp>
#include <boost/spirit/include/qi.hpp>

namespace client {
namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

///////////////////////////////////////////////////////////////////////////////
//  Our calculator grammar
///////////////////////////////////////////////////////////////////////////////
template <typename IT>
class calculator : public qi::grammar<IT, ast::program(), ascii::space_type> {
   public:
    calculator() : calculator::base_type(expression_) {
        using boost::phoenix::function;

        qi::char_type char_;
        qi::lit_type lit_;
        qi::double_type double_;
        qi::_1_type _1;
        qi::_2_type _2;
        qi::_3_type _3;
        qi::_4_type _4;

        expression_ = term_ >> *((char_('+') > term_) | (char_('-') > term_));
        term_ = factor_ >> *((char_('*') > factor_) | (char_('/') > factor_));
        factor_ = double_ | (lit_('(') > expression_ > lit_(')'));

        // Debugging and error handling and reporting support.
        expression_.name("expression");
        term_.name("term");
        factor_.name("factor");

        // Error handling
        function eh = error_handler();
        qi::on_error<qi::fail>(expression_, eh(_1, _2, _3, _4));
    }

   private:
    qi::rule<IT, ast::program(), ascii::space_type> expression_;
    qi::rule<IT, ast::program(), ascii::space_type> term_;
    qi::rule<IT, ast::operand(), ascii::space_type> factor_;
};
}  // namespace client