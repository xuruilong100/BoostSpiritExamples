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

#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <iostream>
#include <list>
#include <string>

namespace client::ast {
///////////////////////////////////////////////////////////////////////////
//  The AST
///////////////////////////////////////////////////////////////////////////
struct tagged {
    std::size_t id_;  // Used to annotate the AST with the iterator position.
                      // This id is used as a key to a map<std::size_t, IT>
                      // (not really part of the AST.)
};

struct nil {};
struct signed_;
struct expression;

struct variable : tagged {
    variable() = default;
    variable(std::string const& name) : name_(name) {}
    std::string name_;
};

using operand = boost::variant<nil,
                               double,
                               variable,
                               boost::recursive_wrapper<signed_>,
                               boost::recursive_wrapper<expression>>;

struct signed_ {
    char sign_;
    operand operand_;
};

struct operation {
    char operator_;
    operand operand_;
};

struct expression {
    operand first_;
    std::list<operation> rest_;
};

struct assignment {
    variable lhs_;
    expression rhs_;
};

struct variable_declaration {
    assignment assign_;
};

using statement = boost::variant<variable_declaration, assignment>;

using statement_list = std::list<statement>;

// print functions for debugging
inline std::ostream& operator<<(std::ostream& out, nil) {
    out << "nil";
    return out;
}
inline std::ostream& operator<<(std::ostream& out, variable const& var) {
    out << var.name_;
    return out;
}
}  // namespace client::ast

BOOST_FUSION_ADAPT_STRUCT(client::ast::signed_,  //
                          (char, sign_)          //
                          (client::ast::operand, operand_))

BOOST_FUSION_ADAPT_STRUCT(client::ast::operation,  //
                          (char, operator_)        //
                          (client::ast::operand, operand_))

BOOST_FUSION_ADAPT_STRUCT(client::ast::expression,        //
                          (client::ast::operand, first_)  //
                          (std::list<client::ast::operation>, rest_))

BOOST_FUSION_ADAPT_STRUCT(client::ast::variable_declaration,  //
                          (client::ast::assignment, assign_))

BOOST_FUSION_ADAPT_STRUCT(client::ast::assignment,       //
                          (client::ast::variable, lhs_)  //
                          (client::ast::expression, rhs_))