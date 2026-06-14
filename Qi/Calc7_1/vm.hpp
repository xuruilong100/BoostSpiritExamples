#pragma once

#ifndef BOOST_SPIRIT_NO_PREDEFINED_TERMINALS
#define BOOST_SPIRIT_NO_PREDEFINED_TERMINALS
#endif

#include <limits>
#include <vector>

namespace client {
///////////////////////////////////////////////////////////////////////////
//  The Virtual Machine
///////////////////////////////////////////////////////////////////////////
enum class byte_code {
    op_neg,  //  negate the top stack entry
    op_add,  //  add top two stack entries
    op_sub,  //  subtract top two stack entries
    op_mul,  //  multiply top two stack entries
    op_div,  //  divide top two stack entries

    op_load,    //  load a variable
    op_store,   //  store a variable
    op_double,  //  push constant double into the stack
    op_stk_adj  //  adjust the stack for local variables
};

class vmachine {
   public:
    explicit vmachine(std::size_t stackSize = 4096)
        : stack_(stackSize, std::numeric_limits<double>::quiet_NaN()) {}

    void execute(const std::vector<byte_code>& code,
                 const std::vector<double>& value,
                 const std::vector<int>& ptr);
    const auto& get_stack() const { return stack_; };

   private:
    std::vector<double> stack_;
};
}  // namespace client