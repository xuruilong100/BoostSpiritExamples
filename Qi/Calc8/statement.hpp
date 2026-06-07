/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#pragma once

#include "annotation.hpp"
#include "error_handler.hpp"
#include "expression.hpp"

namespace client::parser {
///////////////////////////////////////////////////////////////////////////////
//  The statement grammar
///////////////////////////////////////////////////////////////////////////////
template <typename IT>
struct statement : qi::grammar<IT, ast::statement_list(), ascii::space_type> {
    statement(error_handler<IT>& error_handler);

    expression<IT> expr;
    qi::rule<IT, ast::statement_list(), ascii::space_type> statement_list;
    qi::rule<IT, ast::statement_list(), ascii::space_type> compound_statement;

    qi::rule<IT, ast::statement(), ascii::space_type> statement_;
    qi::rule<IT, ast::variable_declaration(), ascii::space_type>
        variable_declaration;
    qi::rule<IT, ast::assignment(), ascii::space_type> assignment;
    qi::rule<IT, ast::if_statement(), ascii::space_type> if_statement;
    qi::rule<IT, ast::while_statement(), ascii::space_type> while_statement;
    qi::rule<IT, std::string(), ascii::space_type> identifier;
};

template <typename IT>
statement<IT>::statement(error_handler<IT>& error_handler)
    : statement::base_type(statement_list), expr(error_handler) {
    qi::_1_type _1;
    // qi::_2_type _2;
    qi::_3_type _3;
    qi::_4_type _4;

    qi::_val_type _val;
    qi::raw_type raw;
    qi::lexeme_type lexeme;
    qi::alpha_type alpha;
    qi::alnum_type alnum;
    qi::lit_type lit;

    using boost::phoenix::function;
    using qi::fail;
    using qi::on_error;
    using qi::on_success;

    using error_handler_function = function<client::error_handler<IT>>;
    using annotation_function = function<client::annotation<IT>>;

    statement_list = +statement_;

    statement_ = variable_declaration | assignment | compound_statement |
                 if_statement | while_statement;

    identifier = !expr.keywords >> raw[lexeme[(alpha | '_') >> *(alnum | '_')]];

    variable_declaration =
        lexeme["var" >> !(alnum | '_')]  // make sure we have whole words
        > &identifier                    // expect an identifier
        > assignment;

    assignment = identifier > '=' > expr > ';';

    if_statement =
        lit("if") > '(' > expr > ')' > statement_ >
        -(lexeme["else" >> !(alnum | '_')]  // make sure we have whole words
          > statement_);

    while_statement = lit("while") > '(' > expr > ')' > statement_;

    compound_statement = lit('{') >> -statement_list >> lit('}');

    // Debugging and error handling and reporting support.
    BOOST_SPIRIT_DEBUG_NODE(statement_list);
    BOOST_SPIRIT_DEBUG_NODE(identifier);
    BOOST_SPIRIT_DEBUG_NODE(variable_declaration);
    BOOST_SPIRIT_DEBUG_NODE(assignment);
    BOOST_SPIRIT_DEBUG_NODE(if_statement);
    BOOST_SPIRIT_DEBUG_NODE(while_statement);

    // Error handling: on error in statement_list, call error_handler.
    on_error<fail>(statement_list, error_handler_function(error_handler)(
                                       "Error! Expecting ", _4, _3));

    // Annotation: on success in assignment, call annotation.
    on_success(assignment, annotation_function(error_handler.iters)(_val, _1));
}
}  // namespace client::parser