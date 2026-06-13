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

    const auto& keywords() const { return keywords_; }

   private:
    qi::rule<IT, ast::expression(), ascii::space_type> expr_;
    qi::rule<IT, ast::expression(), ascii::space_type> equality_expr_;
    qi::rule<IT, ast::expression(), ascii::space_type> relational_expr_;
    qi::rule<IT, ast::expression(), ascii::space_type> logical_expr_;
    qi::rule<IT, ast::expression(), ascii::space_type> additive_expr_;
    qi::rule<IT, ast::expression(), ascii::space_type> multiplicative_expr_;
    qi::rule<IT, ast::operand(), ascii::space_type> unary_expr_;
    qi::rule<IT, ast::operand(), ascii::space_type> primary_expr_;
    qi::rule<IT, std::string(), ascii::space_type> identifier_;

    qi::symbols<char, ast::optoken> equality_op_;
    qi::symbols<char, ast::optoken> relational_op_;
    qi::symbols<char, ast::optoken> logical_op_;
    qi::symbols<char, ast::optoken> additive_op_;
    qi::symbols<char, ast::optoken> multiplicative_op_;
    qi::symbols<char, ast::optoken> unary_op_;

    qi::symbols<char> keywords_;
};

template <typename IT>
expression<IT>::expression(error_handler<IT>& error_handler)
    : expression::base_type(expr_) {
    using enum ast::optoken;

    qi::_1_type _1;
    // qi::_2_type _2;
    qi::_3_type _3;
    qi::_4_type _4;

    qi::char_type char_;
    qi::double_type double_;
    qi::_val_type _val;
    qi::raw_type raw;
    qi::lexeme_type lexeme;
    qi::alpha_type alpha;
    qi::alnum_type alnum;
    qi::bool_type bool_;

    using boost::phoenix::function;
    using qi::fail;
    using qi::on_error;
    using qi::on_success;

    using error_handler_function = function<client::error_handler<IT>>;
    using annotation_function = function<client::annotation<IT>>;

    ///////////////////////////////////////////////////////////////////////
    // Tokens
    logical_op_.add("&&", op_and)("||", op_or);

    equality_op_.add("==", op_equal)("!=", op_not_equal);

    relational_op_.add("<", op_less)("<=", op_less_equal)(">", op_greater)(
        ">=", op_greater_equal);

    additive_op_.add("+", op_plus)("-", op_minus);

    multiplicative_op_.add("*", op_times)("/", op_divide);

    unary_op_.add("+", op_positive)("-", op_negative)("!", op_not);

    keywords_.add("var")("true")("false")("if")("else")("while");

    ///////////////////////////////////////////////////////////////////////
    // Main expression grammar
    expr_ = logical_expr_.alias();

    logical_expr_ = equality_expr_ >> *(logical_op_ > equality_expr_);

    equality_expr_ = relational_expr_ >> *(equality_op_ > relational_expr_);

    relational_expr_ = additive_expr_ >> *(relational_op_ > additive_expr_);

    additive_expr_ =
        multiplicative_expr_ >> *(additive_op_ > multiplicative_expr_);

    multiplicative_expr_ = unary_expr_ >> *(multiplicative_op_ > unary_expr_);

    unary_expr_ = primary_expr_ | (unary_op_ > primary_expr_);

    primary_expr_ = double_ | identifier_ | bool_ | '(' > expr_ > ')';

    identifier_ = !keywords_ >> raw[lexeme[(alpha | '_') >> *(alnum | '_')]];

    ///////////////////////////////////////////////////////////////////////
    // Debugging and error handling and reporting support.
    expr_.name("expr");
    equality_expr_.name("equality_expr");
    relational_expr_.name("relational_expr");
    logical_expr_.name("logical_expr");
    additive_expr_.name("additive_expr");
    multiplicative_expr_.name("multiplicative_expr");
    unary_expr_.name("unary_expr");
    primary_expr_.name("primary_expr");
    identifier_.name("identifier");

    ///////////////////////////////////////////////////////////////////////
    // Error handling: on error in expr, call error_handler.
    error_handler_function eh(error_handler);
    on_error<fail>(expr_, eh("Error! Expecting ", _4, _3));

    ///////////////////////////////////////////////////////////////////////
    // Annotation: on success in primary_expr, call annotation.
    annotation_function af(annotation<IT>(error_handler.iters));
    on_success(primary_expr_, af(_val, _1));
}
}  // namespace client::parser