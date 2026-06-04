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
#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <iostream>
#include <stdexcept>

namespace client::ast {
///////////////////////////////////////////////////////////////////////////
//  The AST
///////////////////////////////////////////////////////////////////////////
struct nil {};
struct program;

using operand = boost::variant<nil, double, boost::recursive_wrapper<program>>;

struct operation {
    char operator_;
    operand operand_;
};

struct program {
    operand first;
    std::list<operation> rest;
};
}  // namespace client::ast

BOOST_FUSION_ADAPT_STRUCT(client::ast::operation,  //
                          (char, operator_)        //
                          (client::ast::operand, operand_))

BOOST_FUSION_ADAPT_STRUCT(client::ast::program,          //
                          (client::ast::operand, first)  //
                          (std::list<client::ast::operation>, rest))

namespace client::ast {
///////////////////////////////////////////////////////////////////////////
//  The AST Printer
///////////////////////////////////////////////////////////////////////////
class printer {
   public:
    using result_type = void;

    result_type operator()(nil) const {}

    result_type operator()(double n) const { std::cout << n; }

    result_type operator()(program const& x) const {
        boost::apply_visitor(*this, x.first);
        for (operation const& oper : x.rest) {
            std::cout << ' ';
            this->print(oper);
        }
    }

   private:
    result_type print(operation const& x) const {
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
class eval {
   public:
    using result_type = double;

    result_type operator()(nil) const { return 0.0; }

    result_type operator()(double n) const { return n; }

    result_type operator()(program const& x) const {
        result_type state = boost::apply_visitor(*this, x.first);
        for (operation const& oper : x.rest) {
            state = this->evalue(oper, state);
        }
        return state;
    }

   private:
    result_type evalue(operation const& x, double lhs) const {
        result_type rhs = boost::apply_visitor(*this, x.operand_);
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