/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include "vm.hpp"

namespace client {
void vmachine::execute(std::vector<double> const& code) {
    auto code_ptr = code.begin();
    stack_ptr_ = stack_.begin();

    while (code_ptr != code.end()) {
        auto raw_op_code = *code_ptr;
        ++code_ptr;
        auto op_code = static_cast<byte_code>(raw_op_code);

        switch (op_code) {
            using enum byte_code;
            case op_neg:
                stack_ptr_[-1] = -stack_ptr_[-1];
                break;

            case op_add:
                --stack_ptr_;
                stack_ptr_[-1] += stack_ptr_[0];
                break;

            case op_sub:
                --stack_ptr_;
                stack_ptr_[-1] -= stack_ptr_[0];
                break;

            case op_mul:
                --stack_ptr_;
                stack_ptr_[-1] *= stack_ptr_[0];
                break;

            case op_div:
                --stack_ptr_;
                stack_ptr_[-1] /= stack_ptr_[0];
                break;

            case op_double: {
                double value = *code_ptr;
                ++code_ptr;
                *stack_ptr_ = value;
                ++stack_ptr_;
            } break;
        }
    }
}
}  // namespace client