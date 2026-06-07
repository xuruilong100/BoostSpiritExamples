#include "vm.hpp"

namespace client {
void vmachine::execute(std::vector<double> const& code) {
    auto pc = code.begin();
    auto locals = stack_.begin();
    stack_ptr_ = stack_.begin();

    while (pc != code.end()) {
        auto raw_opcode = *pc;
        ++pc;
        auto opcode = static_cast<byte_code>(raw_opcode);

        switch (opcode) {
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

            case op_load: {
                raw_opcode = *pc;
                ++pc;
                auto idx = static_cast<int>(raw_opcode);
                *stack_ptr_ = locals[idx];
                ++stack_ptr_;
            } break;

            case op_store: {
                --stack_ptr_;
                raw_opcode = *pc;
                ++pc;
                auto idx = static_cast<int>(raw_opcode);
                locals[idx] = stack_ptr_[0];
            } break;

            case op_double: {
                double value = *pc;
                ++pc;
                *stack_ptr_ = value;
                ++stack_ptr_;
            } break;

            case op_stk_adj: {
                raw_opcode = *pc;
                ++pc;
                stack_ptr_ = stack_.begin() + static_cast<int>(raw_opcode);
            } break;
        }
    }
}
}  // namespace client