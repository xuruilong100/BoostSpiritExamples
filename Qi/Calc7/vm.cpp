/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include "vm.hpp"

namespace client {
void vmachine::execute(std::vector<double> const& code) {
    auto code_ptr = code.begin();
    auto locals = stack_.begin();
    auto stack_ptr = stack_.begin();

    while (code_ptr != code.end()) {
        auto raw_opcode = *code_ptr;
        ++code_ptr;
        auto opcode = static_cast<byte_code>(raw_opcode);

        switch (opcode) {
            using enum byte_code;
            case op_neg:
                stack_ptr[-1] = -stack_ptr[-1];
                break;

            case op_add:
                --stack_ptr;
                stack_ptr[-1] += stack_ptr[0];
                break;

            case op_sub:
                --stack_ptr;
                stack_ptr[-1] -= stack_ptr[0];
                break;

            case op_mul:
                --stack_ptr;
                stack_ptr[-1] *= stack_ptr[0];
                break;

            case op_div:
                --stack_ptr;
                stack_ptr[-1] /= stack_ptr[0];
                break;

            case op_load: {
                raw_opcode = *code_ptr;
                ++code_ptr;
                auto idx = static_cast<int>(raw_opcode);
                *stack_ptr = locals[idx];
                ++stack_ptr;
            } break;

            case op_store: {
                --stack_ptr;
                raw_opcode = *code_ptr;
                ++code_ptr;
                auto idx = static_cast<int>(raw_opcode);
                locals[idx] = stack_ptr[0];
            } break;

            case op_double: {
                double value = *code_ptr;
                ++code_ptr;
                *stack_ptr = value;
                ++stack_ptr;
            } break;

            case op_stk_adj: {
                raw_opcode = *code_ptr;
                ++code_ptr;
                stack_ptr = stack_.begin() + static_cast<int>(raw_opcode);
            } break;
        }
    }
}
}  // namespace client