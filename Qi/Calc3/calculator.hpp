#pragma once

#ifndef BOOST_SPIRIT_NO_PREDEFINED_TERMINALS
#define BOOST_SPIRIT_NO_PREDEFINED_TERMINALS
#endif

#include <boost/phoenix/operator.hpp>
#include <boost/spirit/include/qi.hpp>
#include <iostream>

namespace client {
namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

template <typename IT>
class calculator : public qi::grammar<IT, double, ascii::space_type> {
   public:
    calculator() : calculator::base_type(expression_) {
        qi::_val_type _val;
        qi::_1_type _1;
        qi::double_type double_;
        qi::char_type char_;

        expression_ = term_[_val = _1] >> *((char_('+') >> term_[_val += _1]) |
                                            (char_('-') >> term_[_val -= _1]));

        term_ = factor_[_val = _1] >> *((char_('*') >> factor_[_val *= _1]) |
                                        (char_('/') >> factor_[_val /= _1]));

        factor_ = double_[_val = _1] |
                  char_('(') >> expression_[_val = _1] >> char_(')');
    }

   private:
    qi::rule<IT, double, ascii::space_type> expression_;
    qi::rule<IT, double, ascii::space_type> term_;
    qi::rule<IT, double, ascii::space_type> factor_;
};
}  // namespace client