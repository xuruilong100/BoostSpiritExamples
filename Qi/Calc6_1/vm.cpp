#include "vm.hpp"

namespace client {
void vmachine::execute(std::vector<byte_code> const& code,
                       std::vector<double> const& value) {
    auto code_ptr = code.begin();
    auto value_ptr = value.begin();
    stack_ptr_ = stack_.begin();

    while (code_ptr != code.end()) {
        auto op_code = *code_ptr;
        ++code_ptr;

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
                double val = *value_ptr;
                ++value_ptr;
                *stack_ptr_ = val;
                ++stack_ptr_;
                break;
            }
        }
    }
}
}  // namespace client