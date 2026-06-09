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
#include <boost/spirit/include/qi.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <iostream>
#include <stdexcept>

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
    operand first_;
    std::list<operation> rest_;
};
}  // namespace client::ast

BOOST_FUSION_ADAPT_STRUCT(client::ast::operation,  //
                          (char, operator_)        //
                          (client::ast::operand, operand_))

BOOST_FUSION_ADAPT_STRUCT(client::ast::expression,        //
                          (client::ast::operand, first_)  //
                          (std::list<client::ast::operation>, rest_))

namespace client::ast {
///////////////////////////////////////////////////////////////////////////
//  The AST Printer
///////////////////////////////////////////////////////////////////////////
class printer {
   public:
    using result_type = void;

    void operator()(nil) const {}

    void operator()(double n) const { std::cout << n; }

    void operator()(expression const& x) const {
        boost::apply_visitor(*this, x.first_);
        for (operation const& oper : x.rest_) {
            std::cout << ' ';
            this->print(oper);
        }
    }

   private:
    void print(operation const& x) const {
        boost::apply_visitor(*this, x.operand_);
        switch (x.operator_) {
            case '+':
                std::cout << " add";
                break;
            case '-':
                std::cout << " subt";
                break;
            case '*':
                std::cout << " mult";
                break;
            case '/':
                std::cout << " div";
                break;
            default:
                throw std::runtime_error("Invalid operator");
        }
    }
};

///////////////////////////////////////////////////////////////////////////
//  The AST evaluator
///////////////////////////////////////////////////////////////////////////
class evaluator {
   public:
    using result_type = double;

    double operator()(nil) const { return 0.0; }

    double operator()(double n) const { return n; }

    double operator()(expression const& x) const {
        double state = boost::apply_visitor(*this, x.first_);
        for (operation const& oper : x.rest_) {
            state = this->evalue(oper, state);
        }
        return state;
    }

   private:
    double evalue(operation const& x, double lhs) const {
        double rhs = boost::apply_visitor(*this, x.operand_);
        switch (x.operator_) {
            case '+':
                return lhs + rhs;
            case '-':
                return lhs - rhs;
            case '*':
                return lhs * rhs;
            case '/':
                return lhs / rhs;
            default:
                throw std::runtime_error("Invalid operator");
        }
    }
};
}  // namespace client::ast