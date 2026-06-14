#pragma once

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

    op_not,  //  boolean negate the top stack entry
    op_eq,   //  compare the top two stack entries for ==
    op_neq,  //  compare the top two stack entries for !=
    op_lt,   //  compare the top two stack entries for <
    op_lte,  //  compare the top two stack entries for <=
    op_gt,   //  compare the top two stack entries for >
    op_gte,  //  compare the top two stack entries for >=

    op_and,  //  logical and top two stack entries
    op_or,   //  logical or top two stack entries

    op_load,   //  load a variable
    op_store,  //  store a variable

    op_double,  //  push constant double into the stack
    op_true,    //  push constant 0 into the stack
    op_false,   //  push constant 1 into the stack

    op_jump_if,  //  jump to a relative position in the code if top stack
                 //  evaluates to false
    op_jump,     //  jump to a relative position in the code

    op_stk_adj,  // adjust the stack (for args and locals)
};

class vmachine {
   public:
    explicit vmachine(std::size_t stackSize = 4096)
        : stack_(stackSize, std::numeric_limits<double>::quiet_NaN()) {}

    void execute(std::vector<byte_code> const& code,
                 std::vector<int> const& ptr,
                 std::vector<double> const& value);
    const auto& get_stack() const { return stack_; }

   private:
    std::vector<double> stack_;
};
}  // namespace client