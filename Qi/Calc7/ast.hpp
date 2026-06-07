#pragma once

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
    std::size_t id;  // Used to annotate the AST with the iterator position.
                     // This id is used as a key to a map<std::size_t, IT>
                     // (not really part of the AST.)
};

struct nil {};

struct signed_;

struct expression;

struct variable : tagged {
    variable() = default;
    variable(std::string const& name) : name(name) {}
    std::string name;
};

using operand = boost::variant<nil,
                               double,
                               variable,
                               boost::recursive_wrapper<signed_>,
                               boost::recursive_wrapper<expression>>;

struct signed_ {
    char sign;
    operand operand_;
};

struct operation {
    char operator_;
    operand operand_;
};

struct expression {
    operand first;
    std::list<operation> rest;
};

struct assignment {
    variable lhs;
    expression rhs;
};

struct variable_declaration {
    assignment assign;
};

using statement = boost::variant<variable_declaration, assignment>;

using statement_list = std::list<statement>;

// print functions for debugging
inline std::ostream& operator<<(std::ostream& out, nil) {
    out << "nil";
    return out;
}
inline std::ostream& operator<<(std::ostream& out, variable const& var) {
    out << var.name;
    return out;
}
}  // namespace client::ast

BOOST_FUSION_ADAPT_STRUCT(client::ast::signed_,  //
                          (char, sign)           //
                          (client::ast::operand, operand_))

BOOST_FUSION_ADAPT_STRUCT(client::ast::operation,  //
                          (char, operator_)        //
                          (client::ast::operand, operand_))

BOOST_FUSION_ADAPT_STRUCT(client::ast::expression,       //
                          (client::ast::operand, first)  //
                          (std::list<client::ast::operation>, rest))

BOOST_FUSION_ADAPT_STRUCT(client::ast::variable_declaration,  //
                          (client::ast::assignment, assign))

BOOST_FUSION_ADAPT_STRUCT(client::ast::assignment,      //
                          (client::ast::variable, lhs)  //
                          (client::ast::expression, rhs))