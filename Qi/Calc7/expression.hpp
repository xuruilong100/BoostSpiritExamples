#pragma once

#ifndef BOOST_SPIRIT_NO_PREDEFINED_TERMINALS
#define BOOST_SPIRIT_NO_PREDEFINED_TERMINALS
#endif

///////////////////////////////////////////////////////////////////////////////
// Uncomment this if you want to enable debugging
// #define BOOST_SPIRIT_QI_DEBUG
///////////////////////////////////////////////////////////////////////////////

#include "annotation.hpp"
#include "ast.hpp"
#include "error_handler.hpp"
#include "expression.hpp"
#include <boost/phoenix/function.hpp>
#include <boost/spirit/include/qi.hpp>
#include <vector>

namespace client::parser {
namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

///////////////////////////////////////////////////////////////////////////////
//  The expression grammar
///////////////////////////////////////////////////////////////////////////////
template <typename IT>
class expression
    : public qi::grammar<IT, ast::expression(), ascii::space_type> {
   public:
    expression(error_handler<IT>& error_handler);

   private:
    qi::rule<IT, ast::expression(), ascii::space_type> expr_;
    qi::rule<IT, ast::expression(), ascii::space_type> additive_expr_;
    qi::rule<IT, ast::expression(), ascii::space_type> multiplicative_expr_;
    qi::rule<IT, ast::operand(), ascii::space_type> unary_expr_;
    qi::rule<IT, ast::operand(), ascii::space_type> primary_expr_;
    qi::rule<IT, std::string(), ascii::space_type> identifier_;
};

template <typename IT>
expression<IT>::expression(error_handler<IT>& error_handler)
    : expression::base_type(expr_) {
    qi::_1_type _1;
    // qi::_2_type _2;
    qi::_3_type _3;
    qi::_4_type _4;

    qi::char_type char_;
    qi::lit_type lit_;
    qi::double_type double_;
    qi::_val_type _val;
    qi::raw_type raw;
    qi::lexeme_type lexeme;
    qi::alpha_type alpha;
    qi::alnum_type alnum;

    using boost::phoenix::function;
    using qi::fail;
    using qi::on_error;
    using qi::on_success;

    using error_handler_function = function<client::error_handler<IT>>;
    using annotation_function = function<client::annotation<IT>>;

    expr_ = additive_expr_.alias();

    additive_expr_ =
        multiplicative_expr_ >> *((char_('+') > multiplicative_expr_) |
                                  (char_('-') > multiplicative_expr_));

    multiplicative_expr_ = unary_expr_ >> *((char_('*') > unary_expr_) |
                                            (char_('/') > unary_expr_));

    unary_expr_ = primary_expr_ | (char_('-') > primary_expr_) |
                  (char_('+') > primary_expr_);

    primary_expr_ = double_ | identifier_ | lit_('(') > expr_ > lit_(')');

    identifier_ = raw[lexeme[(alpha | '_') >> *(alnum | '_')]];

    // Debugging and error handling and reporting support.
    expr_.name("expr");
    additive_expr_.name("additive_expr");
    multiplicative_expr_.name("multiplicative_expr");
    unary_expr_.name("unary_expr");
    primary_expr_.name("primary_expr");
    identifier_.name("identifier");

    // Error handling: on error in expr, call error_handler.
    error_handler_function eh(error_handler);
    on_error<fail>(expr_, eh("Error! Expecting ", _4, _3));

    // Annotation: on success in primary_expr_, call annotation.
    annotation_function af(client::annotation<IT>(error_handler.iters));
    on_success(primary_expr_, af(_val, _1));
}
}  // namespace client::parser