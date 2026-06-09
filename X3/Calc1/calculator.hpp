#pragma once

#include <boost/spirit/home/x3.hpp>

namespace client {
namespace x3 = boost::spirit::x3;

using x3::char_;
using x3::double_;

struct expression_class;
struct term_class;
struct factor_class;

using expression_rule = x3::rule<expression_class>;
using term_rule = x3::rule<term_class>;
using factor_rule = x3::rule<factor_class>;

const expression_rule expression = "expression";
const term_rule term = "term";
const factor_rule factor = "factor";

const auto expression_def = term >>
                            *((char_('+') >> term) | (char_('-') >> term));

const auto term_def = factor >>
                      *((char_('*') >> factor) | (char_('/') >> factor));

const auto factor_def = double_ | char_('(') >> expression >> char_(')');

BOOST_SPIRIT_DEFINE(expression)

BOOST_SPIRIT_DEFINE(term)

BOOST_SPIRIT_DEFINE(factor)

const auto& calculator = expression;
}  // namespace client