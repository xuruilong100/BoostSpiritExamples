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
    void execute(std::vector<double> const& code);

   private:
    std::vector<double> stack_;
    std::vector<double>::iterator stack_ptr_;
};
}  // namespace client