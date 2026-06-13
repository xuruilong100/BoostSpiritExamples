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
    explicit vmachine(std::size_t stackSize = 4096) : stack_(stackSize) {}

    void execute(const std::vector<double>& code);
    const auto& get_stack() const { return stack_; };

   private:
    std::vector<double> stack_;
};
}  // namespace client