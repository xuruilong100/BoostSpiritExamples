/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include "compiler.hpp"
#include "vm.hpp"
#include <boost/assert.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <set>

namespace client::code_gen {
const std::size_t* program::find_var(std::string const& name) const {
    auto i = variables_.find(name);
    if (i == variables_.end()) {
        return nullptr;
    }
    return &(i->second);
}

void program::add_var(std::string const& name) {
    std::size_t n = variables_.size();
    variables_[name] = n;
}

void program::print_variables(std::vector<double> const& stack) const {
    for (const auto& p : variables_) {
        std::cout << "    " << p.first << ": " << stack[p.second] << std::endl;
    }
}

void program::print_assembler() const {
    auto pc = code_.begin();

    std::vector<std::string> locals(variables_.size());

    for (const auto& p : variables_) {
        locals[p.second] = p.first;
        std::cout << "      local       " << p.first << ", @" << p.second
                  << std::endl;
    }

    std::map<std::size_t, std::string> lines;
    std::set<std::size_t> jumps;

    while (pc != code_.end()) {
        std::string line;
        std::size_t address = std::distance(code_.begin(), pc);

        auto raw_opcode = *pc;
        ++pc;
        auto opcode = static_cast<byte_code>(raw_opcode);

        switch (opcode) {
            using enum byte_code;
            case op_neg:
                line += "      op_neg";
                break;

            case op_not:
                line += "      op_not";
                break;

            case op_add:
                line += "      op_add";
                break;

            case op_sub:
                line += "      op_sub";
                break;

            case op_mul:
                line += "      op_mul";
                break;

            case op_div:
                line += "      op_div";
                break;

            case op_eq:
                line += "      op_eq";
                break;

            case op_neq:
                line += "      op_neq";
                break;

            case op_lt:
                line += "      op_lt";
                break;

            case op_lte:
                line += "      op_lte";
                break;

            case op_gt:
                line += "      op_gt";
                break;

            case op_gte:
                line += "      op_gte";
                break;

            case op_and:
                line += "      op_and";
                break;

            case op_or:
                line += "      op_or";
                break;

            case op_load: {
                auto idx = static_cast<int>(*pc);
                ++pc;
                line += "      op_load     ";
                line += locals[idx];
            } break;

            case op_store: {
                auto idx = static_cast<int>(*pc);
                ++pc;
                line += "      op_store    ";
                line += locals[idx];
            } break;

            case op_double: {
                auto x = static_cast<double>(*pc);
                ++pc;
                line += "      op_double      ";
                line += boost::lexical_cast<std::string>(x);
            } break;

            case op_true:
                line += "      op_true";
                break;

            case op_false:
                line += "      op_false";
                break;

            case op_jump: {
                auto idx = static_cast<int>(*pc);
                ++pc;
                line += "      op_jump     ";
                std::size_t pos = std::distance(code_.begin(), pc) + idx;
                if (pos == code_.size())
                    line += "end";
                else
                    line += boost::lexical_cast<std::string>(pos);
                jumps.insert(pos);
            } break;

            case op_jump_if: {
                auto idx = static_cast<int>(*pc);
                ++pc;
                line += "      op_jump_if  ";
                std::size_t pos = std::distance(code_.begin(), pc) + idx;
                if (pos == code_.size())
                    line += "end";
                else
                    line += boost::lexical_cast<std::string>(pos);
                jumps.insert(pos);
            } break;

            case op_stk_adj: {
                auto idx = static_cast<int>(*pc);
                ++pc;
                line += "      op_stk_adj  ";
                line += boost::lexical_cast<std::string>(idx);
            } break;
            default:
                // op_call and op_return are not used yet
                throw std::runtime_error("Unknown opcode");
        }
        lines[address] = line;
    }

    std::cout << "start:" << std::endl;

    for (const auto& [_1, _2] : lines) {
        if (jumps.contains(_1)) {
            std::cout << _1 << ':' << std::endl;
        }
        std::cout << _2 << std::endl;
    }

    std::cout << "end:" << std::endl;
}

bool compiler::operator()(double x) const {
    using enum byte_code;
    program.op(op_double, x);
    return true;
}

bool compiler::operator()(bool x) const {
    using enum byte_code;
    program.op(x ? op_true : op_false);
    return true;
}

bool compiler::operator()(ast::variable const& x) const {
    using enum byte_code;
    const auto* p = program.find_var(x.name);
    if (p == nullptr) {
        std::cout << x.id << std::endl;
        error_handler(x.id, "Undeclared variable: " + x.name);
        return false;
    }
    program.op(op_load, static_cast<double>(*p));
    return true;
}

bool compiler::operator()(ast::operation const& x) const {
    if (!boost::apply_visitor(*this, x.operand_)) {
        return false;
    }
    switch (x.operator_) {
        using enum ast::optoken;
        case op_plus:
            program.op(byte_code::op_add);
            break;
        case op_minus:
            program.op(byte_code::op_sub);
            break;
        case op_times:
            program.op(byte_code::op_mul);
            break;
        case op_divide:
            program.op(byte_code::op_div);
            break;
        case op_equal:
            program.op(byte_code::op_eq);
            break;
        case op_not_equal:
            program.op(byte_code::op_neq);
            break;
        case op_less:
            program.op(byte_code::op_lt);
            break;
        case op_less_equal:
            program.op(byte_code::op_lte);
            break;
        case op_greater:
            program.op(byte_code::op_gt);
            break;
        case op_greater_equal:
            program.op(byte_code::op_gte);
            break;
        case op_and:
            program.op(byte_code::op_and);
            break;
        case op_or:
            program.op(byte_code::op_or);
            break;
        default:
            return false;
    }
    return true;
}

bool compiler::operator()(ast::unary const& x) const {
    if (!boost::apply_visitor(*this, x.operand_)) {
        return false;
    }
    switch (x.operator_) {
        using enum ast::optoken;
        case op_negative:
            program.op(byte_code::op_neg);
            break;
        case op_not:
            program.op(byte_code::op_not);
            break;
        case op_positive:
            break;
        default:
            return false;
    }
    return true;
}

bool compiler::operator()(ast::expression const& x) const {
    if (!boost::apply_visitor(*this, x.first))
        return false;
    for (const auto& oper : x.rest) {
        if (!(*this)(oper)) {
            return false;
        }
    }
    return true;
}

bool compiler::operator()(ast::assignment const& x) const {
    using enum byte_code;
    if (!(*this)(x.rhs))
        return false;
    const auto* p = program.find_var(x.lhs.name);
    if (p == nullptr) {
        std::cout << x.lhs.id << std::endl;
        error_handler(x.lhs.id, "Undeclared variable: " + x.lhs.name);
        return false;
    }
    program.op(op_store, static_cast<double>(*p));
    return true;
}

bool compiler::operator()(ast::variable_declaration const& x) const {
    using enum byte_code;
    const auto* p = program.find_var(x.assign.lhs.name);
    if (p != nullptr) {
        std::cout << x.assign.lhs.id << std::endl;
        error_handler(x.assign.lhs.id,
                      "Duplicate variable: " + x.assign.lhs.name);
        return false;
    }
    bool r = (*this)(x.assign.rhs);
    if (r) {  // don't add the variable if the RHS fails
        program.add_var(x.assign.lhs.name);
        program.op(op_store,
                   static_cast<double>(*program.find_var(x.assign.lhs.name)));
    }
    return r;
}

bool compiler::operator()(ast::statement const& x) const {
    return boost::apply_visitor(*this, x);
}

bool compiler::operator()(ast::statement_list const& x) const {
    for (const auto& s : x) {
        if (!(*this)(s))
            return false;
    }
    return true;
}

bool compiler::operator()(ast::if_statement const& x) const {
    using enum byte_code;
    if (!(*this)(x.condition))
        return false;
    program.op(op_jump_if, 0);              // we shall fill this (0) in later
    std::size_t skip = program.size() - 1;  // mark its position
    if (!(*this)(x.then))
        return false;
    program[skip] = static_cast<double>(program.size() - skip);
    // now we know where to jump to (after the if branch)

    if (x.else_) {               // We got an else
        program[skip] += 2;      // adjust for the "else" jump
        program.op(op_jump, 0);  // we shall fill this (0) in later
        std::size_t exit = program.size() - 1;  // mark its position
        if (!(*this)(*x.else_))
            return false;
        program[exit] = static_cast<double>(program.size() - exit);
        // now we know where to jump to (after the else branch)
    }

    return true;
}

bool compiler::operator()(ast::while_statement const& x) const {
    using enum byte_code;
    std::size_t loop = program.size();  // mark our position
    if (!(*this)(x.condition))
        return false;
    program.op(op_jump_if, 0);              // we shall fill this (0) in later
    std::size_t exit = program.size() - 1;  // mark its position
    if (!(*this)(x.body))
        return false;
    program.op(op_jump,
               static_cast<double>(loop - 1) -
                   static_cast<double>(program.size()));  // loop back
    program[exit] = static_cast<double>(program.size() - exit);
    // now we know where to jump to (to exit the loop)
    return true;
}

bool compiler::start(ast::statement_list const& x) const {
    using enum byte_code;
    program.clear();
    // op_stk_adj 0 for now. we'll know how many variables we'll have later
    program.op(op_stk_adj, 0);

    if (!(*this)(x)) {
        program.clear();
        return false;
    }
    program[1] = static_cast<double>(program.nvars());
    // now store the actual number of variables
    return true;
}
}  // namespace client::code_gen