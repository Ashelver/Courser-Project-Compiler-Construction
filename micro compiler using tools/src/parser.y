/**
 * --------------------------------------
 * CUHK-SZ CSC4180: Compiler Construction
 * Assignment 1: Micro Language Compiler
 * --------------------------------------
 * Author: HaoLUO
 * Date: February 26th, 2024
 * 
 * This file implements some syntax analysis rules and works as a parser
 * The grammar tree is generated based on the rules and MIPS Code is generated
 * based on the grammar tree and the added structures and functions implemented
 * in File: added_structure_function.c
 */

%{
/* C declarations used in actions */
#include <cstdio>     
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <ctype.h>

#include "node.hpp"

int yyerror (char *s);

int yylex();

extern int cst_only;

Node* root_node = nullptr;
%}

// Define yylval data types with %union
%union {
	int intval;
	const char* strval;
	struct Node* nodeval;
}


// Define terminal symbols with %token. Remember to set the type.
%token <intval> TOK_INTLITERAL
%token <strval> TOK_BEGIN TOK_END TOK_READ TOK_WRITE TOK_LPAREN TOK_RPAREN TOK_SEMICOLON TOK_COMMA TOK_ASSIGNOP TOK_PLUSOP TOK_MINUSOP TOK_ID TOK_SCANEOF


// Start Symbol
%start start

// Define Non-Terminal Symbols with %type. Remember to set the type.
%type <nodeval> program statement_list statement id_list expression_list expression primary

%%
/**
 * Format:
 * Non-Terminal  :  [Non-Terminal, Terminal]+ (production rule 1)   { parser actions in C++ }
 *                  | [Non-Terminal, Terminal]+ (production rule 2) { parser actions in C++ }
 *                  ;
 */


// Production rule here
// The tree generation logic should be in the operation block of each production rule
start   : program TOK_SCANEOF {
			if (cst_only == 1){
				// cst
				root_node = new Node(SymbolClass::START);
				root_node->append_child($1);
				root_node->append_child(new Node(SymbolClass::SCANEOF));
			} else {
				// ast
				root_node = $1;
			}
			
			return 0;
		};

program : TOK_BEGIN statement_list TOK_END {
			$$ = new Node(SymbolClass::PROGRAM);
			if (cst_only == 1){
				// cst
				$$->append_child(new Node(SymbolClass::BEGIN_));
				$$->append_child($2);
				$$->append_child(new Node(SymbolClass::END));
			} else {
				// ast
				$$->append_child($2);
			}
		};

statement_list  : statement {
					$$ = new Node(SymbolClass::STATEMENT_LIST);
					$$ ->append_child($1);
				}
				| statement_list statement  {
					if (cst_only == 1){
						// cst
						$$ = new Node(SymbolClass::STATEMENT_LIST);
						$$->append_child($1);
						$$->append_child($2);
					} else {
						// ast
						$1->append_child($2);
						$$ = $1;
					}
				};

statement   : TOK_ID TOK_ASSIGNOP expression TOK_SEMICOLON {
				if (cst_only == 1) {
					// cst
					$$ = new Node(SymbolClass::STATEMENT);
					$$->append_child(new Node(SymbolClass::ID));
					$$->append_child(new Node(SymbolClass::ASSIGNOP));
					$$->append_child($3);
					$$->append_child(new Node(SymbolClass::SEMICOLON));
				} else {
					// ast
					$$ = new Node(SymbolClass::ASSIGNOP,$2);
					$$->append_child(new Node(SymbolClass::ID,$1));
					$$->append_child($3);
				}
			}
			| TOK_READ TOK_LPAREN id_list TOK_RPAREN TOK_SEMICOLON {
				if (cst_only == 1){
					// cst
					$$ = new Node(SymbolClass::STATEMENT);
					$$->append_child(new Node(SymbolClass::READ));
					$$->append_child(new Node(SymbolClass::LPAREN));
					$$->append_child($3);
					$$->append_child(new Node(SymbolClass::RPAREN));
					$$->append_child(new Node(SymbolClass::SEMICOLON));
				} else {
					// ast
					$$ = new Node(SymbolClass::READ, $1);
					$$->children = $3->children;
				}
			}
			| TOK_WRITE TOK_LPAREN expression_list TOK_RPAREN TOK_SEMICOLON {
				if (cst_only == 1){
					// cst
					$$ = new Node(SymbolClass::STATEMENT);
					$$->append_child(new Node(SymbolClass::WRITE));
					$$->append_child(new Node(SymbolClass::LPAREN));
					$$->append_child($3);
					$$->append_child(new Node(SymbolClass::RPAREN));
					$$->append_child(new Node(SymbolClass::SEMICOLON));
				} else {
					// ast
					// different situation
					$$ = new Node(SymbolClass::WRITE, $1);
					if ($3->children.empty() || $3->should_preserver_in_ast()) {
						$$->append_child($3);
					} else {
						$$->children = $3->children;
					}
				}
			};

id_list : TOK_ID {
			$$ = new Node(SymbolClass::ID_LIST);
			if (cst_only == 1){
				// cst
				$$->append_child(new Node(SymbolClass::ID));
			} else {
				// ast
				$$->append_child(new Node(SymbolClass::ID, $1));
			}
		}
		| id_list TOK_COMMA TOK_ID {
			$$ = new Node(SymbolClass::ID_LIST);
			if (cst_only == 1){
				// cst
				$$->append_child(new Node(SymbolClass::ID));
				$$->append_child(new Node(SymbolClass::COMMA));
				$$->append_child($1);
			} else {
				// ast
				// different situation
				if ($1->children.empty()){
					$$ = new Node(SymbolClass::ID_LIST);
					$$->append_child($1);
					$$->append_child(new Node(SymbolClass::ID, $3));
				} else {
					$$->children = $1->children;
					$$->append_child(new Node(SymbolClass::ID, $3));
				}
			}
		};

expression_list : expression {
					if (cst_only == 1){
						// cst
						$$ = new Node(SymbolClass::EXPRESSION_LIST);
						$$->append_child($1);
					} else {
						// ast
						$$ = $1;
					}
				}
				| expression_list TOK_COMMA expression {
					if (cst_only == 1){
						// cst
						$$ = new Node(SymbolClass::COMMA);
						$$->append_child($1);
						$$->append_child(new Node(SymbolClass::COMMA));
						$$->append_child($3);
					} else {
						// ast
						// different situation
						if ($1->children.empty()){
							$$ = new Node(SymbolClass::EXPRESSION_LIST);
							$$->append_child($1);
							$$->append_child($3);
						} else {
							$1->append_child($3);
							$$ = $1;
						}
					}
				};


expression  : primary {
				if (cst_only == 1){
					// cst
					$$ = new Node(SymbolClass::EXPRESSION);
					$$->append_child($1);
				} else {
					// ast
					$$ = $1;
				}
			}
			| expression TOK_PLUSOP primary {
				if (cst_only == 1){
					// cst
					$$ = new Node(SymbolClass::EXPRESSION);
					$$->append_child($1);
					$$->append_child(new Node(SymbolClass::PLUSOP));
					$$->append_child($3);
				} else {
					// ast
					$$ = new Node(SymbolClass::PLUSOP, $2);
					$$->append_child($1);
					$$->append_child($3);
				}
			}
			| expression TOK_MINUSOP primary {
				if (cst_only == 1){
					// cst
					$$ = new Node(SymbolClass::EXPRESSION);
					$$->append_child($1);
					$$->append_child(new Node(SymbolClass::MINUSOP));
					$$->append_child($3);
				} else {
					// ast
					$$ = new Node(SymbolClass::MINUSOP, $2);
					$$->append_child($1);
					$$->append_child($3);
				}				
			};

primary : TOK_LPAREN expression TOK_RPAREN {
			if (cst_only == 1){
				// cst
				$$ = new Node(SymbolClass::PRIMARY);
				$$->append_child(new Node(SymbolClass::LPAREN));
				$$->append_child($2);
				$$->append_child(new Node(SymbolClass::RPAREN));
			} else {
				//ast
				$$ = $2;
			}
		}
		| TOK_ID {
			if (cst_only == 1) {
				$$ = new Node(SymbolClass::PRIMARY);
				$$->append_child(new Node(SymbolClass::ID));
			} else {
				$$ = new Node(SymbolClass::ID, $1);
			}
		}
		| TOK_INTLITERAL {
			if (cst_only == 1){
				// cst
				$$ = new Node(SymbolClass::PRIMARY);
				$$->append_child(new Node(SymbolClass::INTLITERAL));
			} else {
				$$ = new Node(SymbolClass::INTLITERAL, std::to_string($1));

			}
		};

%%

int yyerror(char *s) {
	printf("Syntax Error on line %s\n", s);
	return 0;
}
