/* 

   BLISS Compiler copy verb parser header file (blippr1b.h)
   production definitions

   ----------------------------------------------------------------------------

   Written by Tim Josling
   Copyright (C) 1999, 2000, 2001 Free Software Foundation, Inc.
   
   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 2, or (at your option) any
   later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
   
   In other words, you are welcome to use, share and improve this program.
   You are forbidden to forbid anyone else to use, share and improve
   what you give them.   Help stamp out software-hoarding!  

   ----------------------------------------------------------------------------.  */

/*

 Production names, used in parse tree, consisting of P_ name from
 grammar changed to PR_ plus extras to distinguish productions within
 the one P_ name.  I only need the constituents of copy and replace
 here so that's all there is.  */


PRODUCTION_TOKEN (PR_middle_of_replace_normal_clauses)
PRODUCTION_TOKEN (PR_middle_of_replace_OFF)
PRODUCTION_TOKEN (PR_of_library_opt_empty)
PRODUCTION_TOKEN (PR_of_library_opt_normal)
PRODUCTION_TOKEN (PR_of_or_in)
PRODUCTION_TOKEN (PR_tolerant_period)
PRODUCTION_TOKEN (PR_replacing_clauses_opt_empty)
PRODUCTION_TOKEN (PR_replacing_clauses_opt_normal)
PRODUCTION_TOKEN (PR_replacing_clauses_single)
PRODUCTION_TOKEN (PR_replacing_clauses_multiple)
PRODUCTION_TOKEN (PR_delimited_replacing_clauses_single)
PRODUCTION_TOKEN (PR_delimited_replacing_clauses_multiple)
PRODUCTION_TOKEN (PR_text_name)
PRODUCTION_TOKEN (PR_library_name)
PRODUCTION_TOKEN (PR_string)
PRODUCTION_TOKEN (PR_other_reserved_words)
PRODUCTION_TOKEN (PR_other_reserved_words_no_replace)
PRODUCTION_TOKEN (PR_replacing_clause)
PRODUCTION_TOKEN (PR_delimited_replacing_clause)
PRODUCTION_TOKEN (PR_replace_from_to)
PRODUCTION_TOKEN (PR_replace_from_to_number)
PRODUCTION_TOKEN (PR_pseudo_text_with_text)
PRODUCTION_TOKEN (PR_pseudo_text_without_text)
PRODUCTION_TOKEN (PR_within_pseudo_texts_single)
PRODUCTION_TOKEN (PR_within_pseudo_texts_multiple)
PRODUCTION_TOKEN (PR_within_pseudo_text)
PRODUCTION_TOKEN (PR_identifier_non_function)
PRODUCTION_TOKEN (PR_identifier_function)
PRODUCTION_TOKEN (PR_word_other_reserved_words)
PRODUCTION_TOKEN (PR_subscript_or_ref_mod_none_or_one_or_two_none)
PRODUCTION_TOKEN (PR_subscript_or_ref_mod_none_or_one_or_two_one)
PRODUCTION_TOKEN (PR_subscript_or_ref_mod_none_or_one_or_two_two)
PRODUCTION_TOKEN (PR_subscript_or_ref_mod)
PRODUCTION_TOKEN (PR_function_identifier)
PRODUCTION_TOKEN (PR_of_in_clauses_opt_empty)
PRODUCTION_TOKEN (PR_of_in_clauses_opt_normal)
PRODUCTION_TOKEN (PR_of_in_clauses_single)
PRODUCTION_TOKEN (PR_of_in_clauses_multiple)
PRODUCTION_TOKEN (PR_of_in_clause)
PRODUCTION_TOKEN (PR_stuff_in_parentheses_opt_none)
PRODUCTION_TOKEN (PR_stuff_in_parentheses_opt_exists)
PRODUCTION_TOKEN (PR_stuff_in_parentheses_single)
PRODUCTION_TOKEN (PR_stuff_in_parentheses_multiple)
PRODUCTION_TOKEN (PR_stuff_in_parentheses_nested)
PRODUCTION_TOKEN (PR_identifier_ref_mod)
PRODUCTION_TOKEN (PR_identifier_no_ref_mod)
PRODUCTION_TOKEN (PR_identifier_no_ref_mod_both)
PRODUCTION_TOKEN (PR_identifier_no_ref_mod_subscript)
PRODUCTION_TOKEN (PR_subscript_opt_empty)
PRODUCTION_TOKEN (PR_subscript_opt_exists)
PRODUCTION_TOKEN (PR_identifier_ref_mod_of_in)
PRODUCTION_TOKEN (PR_identifier_ref_mod_subscript)
PRODUCTION_TOKEN (PR_identifier_ref_mod_function)
PRODUCTION_TOKEN (PR_subscript_opt_normal)
PRODUCTION_TOKEN (PR_expression_single)
PRODUCTION_TOKEN (PR_expression_multiple)
PRODUCTION_TOKEN (PR_expression_arithmetic)
PRODUCTION_TOKEN (PR_expression_uminus)
PRODUCTION_TOKEN (PR_expression_unary_opt_empty)
PRODUCTION_TOKEN (PR_expression_unary)
PRODUCTION_TOKEN (PR_arithmetic_expression)
PRODUCTION_TOKEN (PR_expression_uplus)
PRODUCTION_TOKEN (PR_expression_with_paren)
PRODUCTION_TOKEN (PR_function_identifier_ref_mod)
PRODUCTION_TOKEN (PR_function_identifier_no_ref_mod)
PRODUCTION_TOKEN (PR_expressions_opt_empty)
PRODUCTION_TOKEN (PR_expressions_opt_normal)
PRODUCTION_TOKEN (PR_expression_number)
PRODUCTION_TOKEN (PR_function_argument_single)
PRODUCTION_TOKEN (PR_function_argument_multiple)
PRODUCTION_TOKEN (PR_function_argument_expression)
PRODUCTION_TOKEN (PR_function_argument_identifier)
PRODUCTION_TOKEN (PR_function_argument_string)
