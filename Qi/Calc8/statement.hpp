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
#include "error_handler.hpp"
#include "expression.hpp"

namespace client::parser {
///////////////////////////////////////////////////////////////////////////////
//  The statement grammar
///////////////////////////////////////////////////////////////////////////////
template <typename IT>
class statement
    : public qi::grammar<IT, ast::statement_list(), ascii::space_type> {
   public:
    statement(error_handler<IT>& error_handler);

   private:
    expression<IT> expression_;
    qi::rule<IT, ast::statement_list(), ascii::space_type> statement_list_;
    qi::rule<IT, ast::statement_list(), ascii::space_type> compound_statement_;
    qi::rule<IT, ast::statement(), ascii::space_type> statement_;
    qi::rule<IT, ast::variable_declaration(), ascii::space_type>
        variable_declaration_;
    qi::rule<IT, ast::assignment(), ascii::space_type> assignment_;
    qi::rule<IT, ast::if_statement(), ascii::space_type> if_statement_;
    qi::rule<IT, ast::while_statement(), ascii::space_type> while_statement_;
    qi::rule<IT, std::string(), ascii::space_type> identifier_;
};

template <typename IT>
statement<IT>::statement(error_handler<IT>& error_handler)
    : statement::base_type(statement_list_), expression_(error_handler) {
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

    statement_list_ = +statement_;

    statement_ = variable_declaration_ | assignment_ | compound_statement_ |
                 if_statement_ | while_statement_;

    identifier_ =
        !expression_.keywords() >> raw[lexeme[(alpha | '_') >> *(alnum | '_')]];

    variable_declaration_ =
        lexeme["var" >> !(alnum | '_')]  // make sure we have whole words
        > &identifier_                   // expect an identifier
        > assignment_;

    assignment_ = identifier_ > '=' > expression_ > ';';

    if_statement_ =
        lit("if") > '(' > expression_ > ')' > statement_ >
        -(lexeme["else" >> !(alnum | '_')]  // make sure we have whole words
          > statement_);

    while_statement_ = lit("while") > '(' > expression_ > ')' > statement_;

    compound_statement_ = lit('{') >> -statement_list_ >> lit('}');

    // Debugging and error handling and reporting support.
    statement_list_.name("statement_list");
    identifier_.name("identifier");
    variable_declaration_.name("variable_declaration");
    assignment_.name("assignment");
    if_statement_.name("if_statement");
    while_statement_.name("while_statement");

    // Error handling: on error in statement_list, call error_handler.
    error_handler_function eh(error_handler);
    on_error<fail>(statement_list_, eh("Error! Expecting ", _4, _3));

    // Annotation: on success in assignment, call annotation.
    annotation_function af(client::annotation<IT>(error_handler.iters));
    on_success(assignment_, af(_val, _1));
}
}  // namespace client::parser