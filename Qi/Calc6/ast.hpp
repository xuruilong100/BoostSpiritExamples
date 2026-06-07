/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#pragma once

#ifndef BOOST_SPIRIT_NO_PREDEFINED_TERMINALS
#define BOOST_SPIRIT_NO_PREDEFINED_TERMINALS
#endif

#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <iostream>
#include <list>

namespace client::ast {
///////////////////////////////////////////////////////////////////////////
//  The AST
///////////////////////////////////////////////////////////////////////////
struct nil {};
struct expression;

using operand =
    boost::variant<nil, double, boost::recursive_wrapper<expression>>;

struct operation {
    char operator_;
    operand operand_;
};

struct expression {
    operand first;
    std::list<operation> rest;
};

// print function for debugging
inline std::ostream& operator<<(std::ostream& out, nil) {
    out << "nil";
    return out;
}
}  // namespace client::ast

BOOST_FUSION_ADAPT_STRUCT(client::ast::operation,  //
                          (char, operator_)        //
                          (client::ast::operand, operand_))

BOOST_FUSION_ADAPT_STRUCT(client::ast::expression,       //
                          (client::ast::operand, first)  //
                          (std::list<client::ast::operation>, rest))