#pragma once

#include "ast.hpp"
#include "error_handler.hpp"
#include "vm.hpp"
#include <boost/function.hpp>
#include <boost/phoenix/core.hpp>
#include <boost/phoenix/function.hpp>
#include <boost/phoenix/operator.hpp>
#include <limits>
#include <map>
#include <vector>

namespace client::code_gen {
///////////////////////////////////////////////////////////////////////////
//  The Program
///////////////////////////////////////////////////////////////////////////
class program {
   public:
    program() = default;
    ~program() = default;

    void op(byte_code code) {
        code_.push_back(code);
        ptr_.push_back(0);
        value_.push_back(nan_);
    }
    void op_ptr(byte_code code, int ptr) {
        code_.push_back(code);
        ptr_.push_back(ptr);
        value_.push_back(nan_);
    }
    void op_value(byte_code code, double value) {
        code_.push_back(code);
        ptr_.push_back(0);
        value_.push_back(value);
    }

    auto& code(std::size_t i) { return code_[i]; }
    const auto& code(std::size_t i) const { return code_[i]; }
    auto& ptr(std::size_t i) { return ptr_[i]; }
    const auto& ptr(std::size_t i) const { return ptr_[i]; }
    auto& value(std::size_t i) { return value_[i]; }
    const auto& value(std::size_t i) const { return value_[i]; }

    void clear() {
        code_.clear();
        ptr_.clear();
        value_.clear();
        variables_.clear();
    }
    std::size_t size() const { return code_.size(); }
    const auto& code() const { return code_; }
    const auto& ptr() const { return ptr_; }
    const auto& value() const { return value_; }

    auto nvars() const { return variables_.size(); }
    const std::size_t* find_var(std::string const& name) const;
    void add_var(std::string const& name);

    void print_variables(std::vector<double> const& stack) const;
    void print_assembler() const;

   private:
    std::map<std::string, std::size_t> variables_;
    std::vector<byte_code> code_;
    std::vector<int> ptr_;
    std::vector<double> value_;

    static inline constexpr double nan_ =
        std::numeric_limits<double>::quiet_NaN();
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

   private:
    client::code_gen::program& program_;
    boost::function<void(int tag, std::string const& what)> error_handler_;
};
}  // namespace client::code_gen