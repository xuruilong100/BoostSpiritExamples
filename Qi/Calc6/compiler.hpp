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
#include "vm.hpp"
#include <stdexcept>
#include <vector>

namespace client {
///////////////////////////////////////////////////////////////////////////
//  The Compiler
///////////////////////////////////////////////////////////////////////////
class compiler {
   public:
    using result_type = void;

    explicit compiler(std::vector<double>& code) : code_(code) {}

    result_type operator()(ast::nil) const {
        // No code to generate for nil
    }

    result_type operator()(double n) const {
        using enum byte_code;
        code_.push_back(static_cast<double>(op_double));
        code_.push_back(n);
    }

    result_type operator()(ast::expression const& x) const {
        boost::apply_visitor(*this, x.first);
        for (const auto& oper : x.rest) {
            this->compile(oper);
        }
    }

   private:
    std::vector<double>& code_;

    result_type compile(ast::operation const& x) const {
        boost::apply_visitor(*this, x.operand_);
        switch (x.operator_) {
            using enum byte_code;
            case '+':
                code_.push_back(static_cast<double>(op_add));
                break;
            case '-':
                code_.push_back(static_cast<double>(op_sub));
                break;
            case '*':
                code_.push_back(static_cast<double>(op_mul));
                break;
            case '/':
                code_.push_back(static_cast<double>(op_div));
                break;
            default:
                throw std::runtime_error("Unknown operator");
        }
    }
};
}  // namespace client