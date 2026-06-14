#pragma once

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
#include <string>
#include <vector>

namespace client::code_gen {
///////////////////////////////////////////////////////////////////////////
//  The Program
///////////////////////////////////////////////////////////////////////////
class program {
   public:
    void op(byte_code code) { code_.push_back(code); }
    void op_value(byte_code code, double value) {
        code_.push_back(code);
        value_.push_back(value);
    }
    void op_ptr(byte_code code, int ptr) {
        code_.push_back(code);
        ptr_.push_back(ptr);
    }

    auto& code(std::size_t i) { return code_[i]; }
    auto const& code(std::size_t i) const { return code_[i]; }
    auto& value(std::size_t i) { return value_[i]; }
    auto const& value(std::size_t i) const { return value_[i]; }
    auto& ptr(std::size_t i) { return ptr_[i]; }
    auto const& ptr(std::size_t i) const { return ptr_[i]; }
    void clear() {
        code_.clear();
        value_.clear();
        ptr_.clear();
        variables_.clear();
    }
    const auto& code() const { return code_; }
    const auto& value() const { return value_; }
    const auto& ptr() const { return ptr_; }

    auto nvars() const { return variables_.size(); }
    const std::size_t* find_var(std::string const& name) const;
    void add_var(std::string const& name);

    void print_variables(std::vector<double> const& stack) const;
    void print_assembler() const;

   private:
    std::map<std::string, std::size_t> variables_;
    std::vector<byte_code> code_;
    std::vector<double> value_;
    std::vector<int> ptr_;
};

///////////////////////////////////////////////////////////////////////////
//  The Compiler
///////////////////////////////////////////////////////////////////////////
class compiler {
   public:
    using result_type = bool;

    template <typename ErrorHandler>
    compiler(program& program, ErrorHandler& error_handler)
        : program_(program) {
        using namespace boost::phoenix::arg_names;
        namespace phx = boost::phoenix;
        using boost::phoenix::function;

        error_handler_ = function<ErrorHandler>(error_handler)(
            "Error! ", _2, phx::cref(error_handler.iters)[_1]);
    }

    bool operator()(ast::nil) const { return false; }
    bool operator()(double x) const;
    bool operator()(ast::variable const& x) const;
    bool operator()(ast::operation const& x) const;
    bool operator()(ast::signed_ const& x) const;
    bool operator()(ast::expression const& x) const;
    bool operator()(ast::assignment const& x) const;
    bool operator()(ast::variable_declaration const& x) const;
    bool operator()(ast::statement_list const& x) const;

   private:
    program& program_;
    boost::function<void(std::size_t tag, std::string const& what)>
        error_handler_;
};
}  // namespace client::code_gen