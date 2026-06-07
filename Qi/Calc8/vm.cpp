/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include "vm.hpp"
#include <boost/assert.hpp>

namespace client {
double vmachine::execute(std::vector<double> const& code,
                         std::vector<double>::const_iterator pc,
                         std::vector<double>::iterator frame_ptr) {
    auto stack_ptr = frame_ptr;

    while (pc != code.end()) {
        auto raw_opcode = *pc;
        ++pc;
        auto opcode = static_cast<byte_code>(raw_opcode);
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
                auto idx = static_cast<int>(*pc);
                ++pc;
                *stack_ptr = frame_ptr[idx];
                ++stack_ptr;
            } break;

            case op_store: {
                --stack_ptr;
                auto idx = static_cast<int>(*pc);
                ++pc;
                frame_ptr[idx] = stack_ptr[0];
            } break;

            case op_double: {
                *stack_ptr = *pc;
                ++stack_ptr;
                ++pc;
            } break;

            case op_true:
                *stack_ptr++ = true;
                break;

            case op_false:
                *stack_ptr++ = false;
                break;

            case op_jump:
                pc += *pc;
                break;

            case op_jump_if:
                if (!bool(stack_ptr[-1]))
                    pc += static_cast<int>(*pc);
                else
                    ++pc;
                --stack_ptr;
                break;

            case op_stk_adj: {
                stack_ptr = stack.begin() + static_cast<int>(*pc);
                ++pc;
            } break;

            case op_call: {
                int nargs = static_cast<int>(*pc);
                ++pc;
                int jump = static_cast<int>(*pc);
                ++pc;

                // a function call is a recursive call to execute
                double r =
                    execute(code, code.begin() + jump, stack_ptr - nargs);

                // cleanup after return from function
                stack_ptr[-nargs] = r;     //  get return value
                stack_ptr -= (nargs - 1);  //  the stack will now contain
                                           //  the return value
            } break;

            case op_return:
                return stack_ptr[-1];
        }
    }
    return -1;
}
}  // namespace client