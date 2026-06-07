/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#pragma once

// Spirit v2.5 allows you to suppress automatic generation
// of predefined terminals to speed up complation. With
// BOOST_SPIRIT_NO_PREDEFINED_TERMINALS defined, you are
// responsible in creating instances of the terminals that
// you need (e.g. see qi::uint_type uint_ below).
#ifndef BOOST_SPIRIT_NO_PREDEFINED_TERMINALS
#define BOOST_SPIRIT_NO_PREDEFINED_TERMINALS
#endif

#include <iostream>
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
    op_call,     // function call
    op_return    // return from function
};

class vmachine {
   public:
    explicit vmachine(std::size_t stackSize = 4096) : stack(stackSize) {}

    double execute(std::vector<double> const& code,         // the program code
                   std::vector<double>::const_iterator pc,  // program counter
                   std::vector<double>::iterator
                       frame_ptr  // start of arguments and locals
    );

    double execute(std::vector<double> const& code) {
        return execute(code, code.begin(), stack.begin());
    };

    std::vector<double> stack;
};
}  // namespace client