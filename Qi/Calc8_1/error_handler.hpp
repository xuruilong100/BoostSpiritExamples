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
#include <string>
#include <vector>

namespace client {
///////////////////////////////////////////////////////////////////////////////
//  The error handler
///////////////////////////////////////////////////////////////////////////////
template <typename IT>
struct error_handler {
    template <typename, typename, typename>
    struct result {
        using type = void;
    };

    error_handler(IT first, IT last) : first(first), last(last) {}

    template <typename Message, typename What>
    void operator()(Message const& message,
                    What const& what,
                    IT err_pos) const {
        int line;
        IT line_start = get_pos(err_pos, line);
        if (err_pos != last) {
            std::cout << message << what << " line " << line << ':'
                      << std::endl;
            std::cout << get_line(line_start) << std::endl;
            for (; line_start != err_pos; ++line_start)
                std::cout << ' ';
            std::cout << '^' << std::endl;
        } else {
            std::cout << "Unexpected end of file. ";
            std::cout << message << what << " line " << line << std::endl;
        }
    }

    IT get_pos(IT err_pos, int& line) const {
        line = 1;
        IT i = first;
        IT line_start = first;
        while (i != err_pos) {
            bool eol = false;
            if (i != err_pos && *i == '\r') {  // CR
                eol = true;
                line_start = ++i;
            }
            if (i != err_pos && *i == '\n') {  // LF
                eol = true;
                line_start = ++i;
            }
            if (eol) {
                ++line;
            } else {
                ++i;
            }
        }
        return line_start;
    }

    std::string get_line(IT err_pos) const {
        IT i = err_pos;
        // position i to the next EOL
        while (i != last && (*i != '\r' && *i != '\n')) {
            ++i;
        }
        return std::string(err_pos, i);
    }

    IT first;
    IT last;
    std::vector<IT> iters;
};
}  // namespace client