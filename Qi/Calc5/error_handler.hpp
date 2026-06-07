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

#include <boost/spirit/include/qi.hpp>
#include <iostream>
#include <string>

namespace client {
namespace qi = boost::spirit::qi;

///////////////////////////////////////////////////////////////////////////////
//  Our error handler
///////////////////////////////////////////////////////////////////////////////
class error_handler {
   public:
    template <typename, typename, typename>
    struct result {
        using type = void;
    };

    template <typename IT>
    void operator()(IT first, IT last, IT err_pos, qi::info const& what) const {
        std::cout << "Error! Expecting " << what  // what failed?
                  << " here: \""
                  << std::string(err_pos, last)  // iterators to error-pos, end
                  << "\"" << std::endl;
    }
};
}  // namespace client