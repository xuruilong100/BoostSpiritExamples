#pragma once

#ifndef BOOST_SPIRIT_NO_PREDEFINED_TERMINALS
#define BOOST_SPIRIT_NO_PREDEFINED_TERMINALS
#endif

#include <vector>

namespace client {
///////////////////////////////////////////////////////////////////////////
//  The Virtual Machine
///////////////////////////////////////////////////////////////////////////
enum class byte_code {
    op_neg,     //  negate the top stack entry
    op_add,     //  add top two stack entries
    op_sub,     //  subtract top two stack entries
    op_mul,     //  multiply top two stack entries
    op_div,     //  divide top two stack entries
    op_double,  //  push constant double into the stack
};

class vmachine {
   public:
    explicit vmachine(unsigned stackSize = 4096)
        : stack_(stackSize), stack_ptr_(stack_.begin()) {}

    double top() const { return stack_ptr_[-1]; };
    void execute(std::vector<byte_code> const& code,
                 std::vector<double> const& value);

   private:
    std::vector<double> stack_;
    std::vector<double>::iterator stack_ptr_;
};
}  // namespace client