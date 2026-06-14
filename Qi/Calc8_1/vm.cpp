/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include "vm.hpp"
#include <boost/assert.hpp>

namespace client {
void vmachine::execute(std::vector<byte_code> const& code,
                       std::vector<int> const& ptr,
                       std::vector<double> const& value) {
    auto code_ptr = code.begin();
    auto ptr_ptr = ptr.begin();
    auto value_ptr = value.begin();

    auto locals = stack_.begin();
    auto stack_ptr = stack_.begin();

    bool jumped = false;

    while (code_ptr != code.end()) {
        auto opcode = *code_ptr;

        switch (opcode) {
            using enum byte_code;
            case op_neg:
                stack_ptr[-1] = -stack_ptr[-1];
                break;

            case op_not:
                stack_ptr[-1] = !bool(stack_ptr[-1]);
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

            case op_eq:
                --stack_ptr;
                stack_ptr[-1] = bool(stack_ptr[-1] == stack_ptr[0]);
                break;

            case op_neq:
                --stack_ptr;
                stack_ptr[-1] = bool(stack_ptr[-1] != stack_ptr[0]);
                break;

            case op_lt:
                --stack_ptr;
                stack_ptr[-1] = bool(stack_ptr[-1] < stack_ptr[0]);
                break;

            case op_lte:
                --stack_ptr;
                stack_ptr[-1] = bool(stack_ptr[-1] <= stack_ptr[0]);
                break;

            case op_gt:
                --stack_ptr;
                stack_ptr[-1] = bool(stack_ptr[-1] > stack_ptr[0]);
                break;

            case op_gte:
                --stack_ptr;
                stack_ptr[-1] = bool(stack_ptr[-1] >= stack_ptr[0]);
                break;

            case op_and:
                --stack_ptr;
                stack_ptr[-1] = bool(stack_ptr[-1]) && bool(stack_ptr[0]);
                break;

            case op_or:
                --stack_ptr;
                stack_ptr[-1] = bool(stack_ptr[-1]) || bool(stack_ptr[0]);
                break;

            case op_load: {
                auto idx = *ptr_ptr;
                *stack_ptr = locals[idx];
                ++stack_ptr;
            } break;

            case op_store: {
                --stack_ptr;
                auto idx = *ptr_ptr;
                locals[idx] = stack_ptr[0];
            } break;

            case op_double: {
                *stack_ptr = *value_ptr;
                ++stack_ptr;
            } break;

            case op_true: {
                *stack_ptr = true;
                ++stack_ptr;
            } break;

            case op_false: {
                *stack_ptr = false;
                ++stack_ptr;
            } break;

            case op_jump: {
                auto idx = *ptr_ptr;
                code_ptr += idx;
                ptr_ptr += idx;
                value_ptr += idx;
                jumped = true;
            } break;

            case op_jump_if:
                if (!bool(stack_ptr[-1])) {
                    auto idx = *ptr_ptr;
                    code_ptr += idx;
                    ptr_ptr += idx;
                    value_ptr += idx;
                    jumped = true;
                }
                --stack_ptr;
                break;

            case op_stk_adj: {
                auto idx = *ptr_ptr;
                stack_ptr = stack_.begin() + idx;
            } break;
        }

        if (jumped) {
            jumped = false;
        } else {
            ++code_ptr;
            ++ptr_ptr;
            ++value_ptr;
        }
    }
}
}  // namespace client