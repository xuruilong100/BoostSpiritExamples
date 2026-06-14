/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include "compiler.hpp"
#include "vm.hpp"
#include <boost/assert.hpp>
#include <boost/foreach.hpp>
#include <boost/variant/apply_visitor.hpp>

namespace client::code_gen {
const std::size_t* program::find_var(std::string const& name) const {
    auto i = variables_.find(name);
    if (i == variables_.end()) {
        return nullptr;
    }
    return &(i->second);
}

void program::add_var(std::string const& name) {
    auto n = variables_.size();
    variables_[name] = n;
}

void program::print_variables(std::vector<double> const& stack) const {
    for (const auto& p : variables_) {
        std::cout << "    " << p.first << ": " << stack[p.second] << std::endl;
    }
}

void program::print_assembler() const {
    std::vector<std::string> locals(variables_.size());

    for (const auto& p : variables_) {
        locals[p.second] = p.first;
        std::cout << "local       " << p.first << ", @" << p.second
                  << std::endl;
    }

    auto pc = code_.begin();
    while (pc != code_.end()) {
        auto raw_opcode = *pc;
        ++pc;
        auto opcode = static_cast<byte_code>(raw_opcode);

        switch (opcode) {
            using enum byte_code;
            case op_neg:
                std::cout << "op_neg" << std::endl;
                break;
            case op_add:
                std::cout << "op_add" << std::endl;
                break;
            case op_sub:
                std::cout << "op_sub" << std::endl;
                break;
            case op_mul:
                std::cout << "op_mul" << std::endl;
                break;
            case op_div:
                std::cout << "op_div" << std::endl;
                break;
            case op_load: {
                auto idx = static_cast<int>(*pc);
                ++pc;
                std::cout << "op_load     " << locals[idx] << std::endl;
            } break;
            case op_store: {
                auto idx = static_cast<int>(*pc);
                ++pc;
                std::cout << "op_store    " << locals[idx] << std::endl;
            } break;
            case op_double: {
                auto value = *pc;
                ++pc;
                std::cout << "op_double   " << value << std::endl;
            } break;
            case op_stk_adj: {
                auto value = *pc;
                ++pc;
                std::cout << "op_stk_adj  " << value << std::endl;
            } break;
        }
    }
}

bool compiler::operator()(double x) const {
    using enum byte_code;
    program_.op(op_double, x);
    return true;
}

bool compiler::operator()(ast::variable const& x) const {
    using enum byte_code;
    const auto* p = program_.find_var(x.name_);
    if (p == nullptr) {
        std::cout << x.id_ << std::endl;
        error_handler_(x.id_, "Undeclared variable: " + x.name_);
        return false;
    }
    program_.op(op_load, static_cast<double>(*p));
    return true;
}

bool compiler::operator()(ast::operation const& x) const {
    using enum byte_code;
    if (!boost::apply_visitor(*this, x.operand_)) {
        return false;
    }
    switch (x.operator_) {
        case '+':
            program_.op(op_add);
            break;
        case '-':
            program_.op(op_sub);
            break;
        case '*':
            program_.op(op_mul);
            break;
        case '/':
            program_.op(op_div);
            break;
    }
    return true;
}

bool compiler::operator()(ast::signed_ const& x) const {
    using enum byte_code;
    if (!boost::apply_visitor(*this, x.operand_)) {
        return false;
    }
    switch (x.sign_) {
        case '-':
            program_.op(op_neg);
            break;
        case '+':
            break;
    }
    return true;
}

bool compiler::operator()(ast::expression const& x) const {
    if (!boost::apply_visitor(*this, x.first_)) {
        return false;
    }
    for (ast::operation const& oper : x.rest_) {
        if (!(*this)(oper)) {
            return false;
        }
    }
    return true;
}

bool compiler::operator()(ast::assignment const& x) const {
    using enum byte_code;
    if (!(*this)(x.rhs_)) {
        return false;
    }
    const auto* p = program_.find_var(x.lhs_.name_);
    if (p == nullptr) {
        std::cout << x.lhs_.id_ << std::endl;
        error_handler_(x.lhs_.id_, "Undeclared variable: " + x.lhs_.name_);
        return false;
    }
    program_.op(op_store, static_cast<double>(*p));
    return true;
}

bool compiler::operator()(ast::variable_declaration const& x) const {
    using enum byte_code;
    if (const auto* p = program_.find_var(x.assign_.lhs_.name_); p != nullptr) {
        std::cout << x.assign_.lhs_.id_ << std::endl;
        error_handler_(x.assign_.lhs_.id_,
                       "Duplicate variable: " + x.assign_.lhs_.name_);
        return false;
    }

    bool r = (*this)(x.assign_.rhs_);
    if (r) {  // don't add the variable if the RHS fails
        program_.add_var(x.assign_.lhs_.name_);
        program_.op(op_store, static_cast<double>(
                                  *program_.find_var(x.assign_.lhs_.name_)));
    }
    return r;
}

bool compiler::operator()(ast::statement_list const& x) const {
    using enum byte_code;
    program_.clear();

    // op_stk_adj 0 for now. we'll know how many variables we'll have later
    program_.op(op_stk_adj, 0);
    for (ast::statement const& s : x) {
        if (!boost::apply_visitor(*this, s)) {
            program_.clear();
            return false;
        }
    }

    // now store the actual number of variables
    program_[1] = static_cast<double>(program_.nvars());

    return true;
}
}  // namespace client::code_gen