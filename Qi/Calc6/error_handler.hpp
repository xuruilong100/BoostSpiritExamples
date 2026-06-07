#pragma once

#ifndef BOOST_SPIRIT_NO_PREDEFINED_TERMINALS
#define BOOST_SPIRIT_NO_PREDEFINED_TERMINALS
#endif

#include <boost/spirit/include/qi.hpp>
#include <iostream>
#include <string>

namespace client {
namespace qi = boost::spirit::qi;

///////////////////////////////////////////////////////////////////////////////
//  The error handler
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