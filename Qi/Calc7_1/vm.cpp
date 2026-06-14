#include "vm.hpp"

namespace client {
void vmachine::execute(const std::vector<byte_code>& code,
                       const std::vector<double>& value,
                       const std::vector<int>& ptr) {
    auto code_ptr = code.begin();
    auto value_ptr = value.begin();
    auto ptr_ptr = ptr.begin();
    auto locals = stack_.begin();
    auto stack_ptr = stack_.begin();

    while (code_ptr != code.end()) {
        auto opcode = *code_ptr;
        ++code_ptr;

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
                auto idx = *ptr_ptr;
                ++ptr_ptr;
                *stack_ptr = locals[idx];
                ++stack_ptr;
            } break;

            case op_store: {
                --stack_ptr;
                auto idx = *ptr_ptr;
                ++ptr_ptr;
                locals[idx] = stack_ptr[0];
            } break;

            case op_double: {
                double val = *value_ptr;
                ++value_ptr;
                *stack_ptr = val;
                ++stack_ptr;
            } break;

            case op_stk_adj: {
                auto idx = *ptr_ptr;
                ++ptr_ptr;
                stack_ptr = stack_.begin() + idx;
            } break;
        }
    }
}
}  // namespace client