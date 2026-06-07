#pragma once

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
struct program {
    void op(byte_code a) { code_.push_back(static_cast<double>(a)); }
    void op(byte_code a, double b) {
        code_.push_back(static_cast<double>(a));
        code_.push_back(b);
    }

    auto& operator[](std::size_t i) { return code_[i]; }
    const auto& operator[](std::size_t i) const { return code_[i]; }
    void clear() {
        code_.clear();
        variables_.clear();
    }
    std::size_t size() const { return code_.size(); }
    const auto& code() const { return code_; }

    auto nvars() const { return variables_.size(); }
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
struct compiler {
    using result_type = bool;

    template <typename ErrorHandler>
    compiler(client::code_gen::program& program, ErrorHandler& error_handler_)
        : program(program) {
        using namespace boost::phoenix::arg_names;
        namespace phx = boost::phoenix;
        using boost::phoenix::function;

        error_handler = function<ErrorHandler>(error_handler_)(
            "Error! ", _2, phx::cref(error_handler_.iters)[_1]);
    }

    bool operator()(ast::nil) const { return false; }
    bool operator()(double x) const;
    bool operator()(bool x) const;
    bool operator()(ast::variable const& x) const;
    bool operator()(ast::operation const& x) const;
    bool operator()(ast::unary const& x) const;
    bool operator()(ast::expression const& x) const;
    bool operator()(ast::assignment const& x) const;
    bool operator()(ast::variable_declaration const& x) const;
    bool operator()(ast::statement_list const& x) const;
    bool operator()(ast::statement const& x) const;
    bool operator()(ast::if_statement const& x) const;
    bool operator()(ast::while_statement const& x) const;

    bool start(ast::statement_list const& x) const;

    client::code_gen::program& program;

    boost::function<void(int tag, std::string const& what)> error_handler;
};
}  // namespace client::code_gen