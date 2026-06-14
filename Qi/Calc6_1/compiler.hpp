#pragma once

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

    compiler(std::vector<byte_code>& code, std::vector<double>& value)
        : code_(code), value_(value) {}

    void operator()(ast::nil) const {
        // No code to generate for nil
    }

    void operator()(double n) const {
        using enum byte_code;
        code_.push_back(op_double);
        value_.push_back(n);
    }

    void operator()(ast::expression const& x) const {
        boost::apply_visitor(*this, x.first_);
        for (const auto& oper : x.rest_) {
            this->compile(oper);
        }
    }

   private:
    std::vector<byte_code>& code_;
    std::vector<double>& value_;

    void compile(ast::operation const& x) const {
        boost::apply_visitor(*this, x.operand_);
        switch (x.operator_) {
            using enum byte_code;
            case '+':
                code_.push_back(op_add);
                break;
            case '-':
                code_.push_back(op_sub);
                break;
            case '*':
                code_.push_back(op_mul);
                break;
            case '/':
                code_.push_back(op_div);
                break;
            default:
                throw std::runtime_error("Unknown operator");
        }
    }
};
}  // namespace client