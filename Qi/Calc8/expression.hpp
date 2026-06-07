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
struct expression : qi::grammar<IT, ast::expression(), ascii::space_type> {
    expression(error_handler<IT>& error_handler);

    qi::rule<IT, ast::expression(), ascii::space_type> expr;
    qi::rule<IT, ast::expression(), ascii::space_type> equality_expr;
    qi::rule<IT, ast::expression(), ascii::space_type> relational_expr;
    qi::rule<IT, ast::expression(), ascii::space_type> logical_expr;
    qi::rule<IT, ast::expression(), ascii::space_type> additive_expr;
    qi::rule<IT, ast::expression(), ascii::space_type> multiplicative_expr;

    qi::rule<IT, ast::operand(), ascii::space_type> unary_expr;
    qi::rule<IT, ast::operand(), ascii::space_type> primary_expr;

    qi::rule<IT, std::string(), ascii::space_type> identifier;

    qi::symbols<char, ast::optoken> equality_op;
    qi::symbols<char, ast::optoken> relational_op;
    qi::symbols<char, ast::optoken> logical_op;
    qi::symbols<char, ast::optoken> additive_op;
    qi::symbols<char, ast::optoken> multiplicative_op;
    qi::symbols<char, ast::optoken> unary_op;

    qi::symbols<char> keywords;
};

template <typename IT>
expression<IT>::expression(error_handler<IT>& error_handler)
    : expression::base_type(expr) {
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
    logical_op.add("&&", op_and)("||", op_or);

    equality_op.add("==", op_equal)("!=", op_not_equal);

    relational_op.add("<", op_less)("<=", op_less_equal)(">", op_greater)(
        ">=", op_greater_equal);

    additive_op.add("+", op_plus)("-", op_minus);

    multiplicative_op.add("*", op_times)("/", op_divide);

    unary_op.add("+", op_positive)("-", op_negative)("!", op_not);

    keywords.add("var")("true")("false")("if")("else")("while");

    ///////////////////////////////////////////////////////////////////////
    // Main expression grammar
    expr = logical_expr.alias();

    logical_expr = equality_expr >> *(logical_op > equality_expr);

    equality_expr = relational_expr >> *(equality_op > relational_expr);

    relational_expr = additive_expr >> *(relational_op > additive_expr);

    additive_expr = multiplicative_expr >> *(additive_op > multiplicative_expr);

    multiplicative_expr = unary_expr >> *(multiplicative_op > unary_expr);

    unary_expr = primary_expr | (unary_op > primary_expr);

    primary_expr = double_ | identifier | bool_ | '(' > expr > ')';

    identifier = !keywords >> raw[lexeme[(alpha | '_') >> *(alnum | '_')]];

    ///////////////////////////////////////////////////////////////////////
    // Debugging and error handling and reporting support.
    BOOST_SPIRIT_DEBUG_NODE(expr);
    BOOST_SPIRIT_DEBUG_NODE(equality_expr);
    BOOST_SPIRIT_DEBUG_NODE(relational_expr);
    BOOST_SPIRIT_DEBUG_NODE(logical_expr);
    BOOST_SPIRIT_DEBUG_NODE(additive_expr);
    BOOST_SPIRIT_DEBUG_NODE(multiplicative_expr);
    BOOST_SPIRIT_DEBUG_NODE(unary_expr);
    BOOST_SPIRIT_DEBUG_NODE(primary_expr);
    BOOST_SPIRIT_DEBUG_NODE(identifier);

    ///////////////////////////////////////////////////////////////////////
    // Error handling: on error in expr, call error_handler.
    on_error<fail>(expr, error_handler_function(error_handler)(
                             "Error! Expecting ", _4, _3));

    ///////////////////////////////////////////////////////////////////////
    // Annotation: on success in primary_expr, call annotation.
    on_success(primary_expr,
               annotation_function(error_handler.iters)(_val, _1));
}
}  // namespace client::parser