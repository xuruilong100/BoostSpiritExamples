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
#include "vm.hpp"
#include <boost/function.hpp>
#include <boost/phoenix/core.hpp>
#include <boost/phoenix/function.hpp>
#include <boost/phoenix/operator.hpp>
#include <map>
#include <vector>

namespace client::code_gen {
///////////////////////////////////////////////////////////////////////////
//  The Program
///////////////////////////////////////////////////////////////////////////
class program {
   public:
    void op(byte_code a) { code_.push_back(static_cast<double>(a)); }
    void op(byte_code a, double b) {
        code_.push_back(static_cast<double>(a));
        code_.push_back(b);
    }

    double& operator[](std::size_t i) { return code_[i]; }
    double const& operator[](std::size_t i) const { return code_[i]; }
    void clear() {
        code_.clear();
        variables_.clear();
    }
    const auto& code() const { return code_; }

    std::size_t nvars() const { return variables_.size(); }
    const std::size_t* find_var(std::string const& name) const;
    void add_var(std::string const& name);

    void print_variables(std::vector<double> const& stack) const;
    void print_assembler() const;

   private:
    std::map<std::string, std::size_t> variables_;
    std::vector<double> code_;
};

///////////////////////////////////////////////////////////////////////////
//  The Compiler
///////////////////////////////////////////////////////////////////////////
class compiler {
   public:
    using result_type = bool;

    template <typename ErrorHandler>
    compiler(client::code_gen::program& program, ErrorHandler& error_handler)
        : program_(program) {
        using namespace boost::phoenix::arg_names;
        namespace phx = boost::phoenix;
        using boost::phoenix::function;

        error_handler_ = function<ErrorHandler>(error_handler)(
            "Error! ", _2, phx::cref(error_handler.iters)[_1]);
    }

    result_type operator()(ast::nil) const { return false; }
    result_type operator()(double x) const;
    result_type operator()(ast::variable const& x) const;
    result_type operator()(ast::operation const& x) const;
    result_type operator()(ast::signed_ const& x) const;
    result_type operator()(ast::expression const& x) const;
    result_type operator()(ast::assignment const& x) const;
    result_type operator()(ast::variable_declaration const& x) const;
    result_type operator()(ast::statement_list const& x) const;

   private:
    client::code_gen::program& program_;
    boost::function<void(std::size_t tag, std::string const& what)>
        error_handler_;
};
}  // namespace client::code_gen