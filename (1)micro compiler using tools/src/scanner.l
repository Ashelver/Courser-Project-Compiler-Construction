/**
 * --------------------------------------
 * CUHK-SZ CSC4180: Compiler Construction
 * Assignment 1: Micro Language Compiler
 * --------------------------------------
 * Author: HaoLUO
 * Date: February 26th, 2024
 * 
 * This file implements some lexical analysis rules to scan the input file 
 * and return the tokens which will be used for parser 
 */


%{
/* C/C++ Stuff (headers, declarations, variables, etc.) */
#include <cstdio>
#include <string>
#include <vector>
#include "parser.hpp"
#include "node.hpp"

int yyerror (char *s);

int yylex();

/**
 * scan_only indicates whehter the flex scanner should print out the token information for debug.
 * The token information is in the following form:
 *   <token-class, lexeme>
 */
extern int scan_only;
%}

SPACES  (\t|\0|\r|\n|\ )+
COMMENT --.*\n
BEGIN_ "begin"
END "end"
READ "read"
WRITE "write"
LPAREN "("
RPAREN ")"
SEMICOLON ";"
COMMA ","
ASSIGNOP ":="
PLUSOP "+"
MINUSOP "-"
ID [a-zA-Z][a-zA-Z0-9_]{0,31}
INTLITERAL  -?[0-9]+

%%

{SPACES} { /* Skip */ }
{COMMENT} { /* Skip */ }
{BEGIN_} {
    if(scan_only == 1) { 
        printf("<BEGIN_, %s>\n", yytext);
        return 1;
    } 
    return TOK_BEGIN; 
}
{END} {
    if(scan_only == 1) {
        printf("<END, %s>\n", yytext);
        return 1;
    }
    return TOK_END;
}
{READ} {
    if(scan_only == 1) {
        printf("<READ, %s>\n", yytext);
        return 1;
    }
    yylval.strval = strdup(yytext);
    return TOK_READ;
}
{WRITE} {
    if(scan_only == 1) {
        printf("<WRITE, %s>\n", yytext);
        return 1;
    }
    yylval.strval = strdup(yytext);
    return TOK_WRITE;
}
{LPAREN} {
    if(scan_only == 1) {
        printf("<LPAREN, %s>\n", yytext);
        return 1;
    }
    return TOK_LPAREN;
}
{RPAREN} {
    if(scan_only == 1) {
        printf("<RPAREN, %s>\n", yytext);
        return 1;
    }
    return TOK_RPAREN;
}
{SEMICOLON} {
    if(scan_only == 1) {
        printf("<SEMICOLON, %s>\n", yytext);
        return 1;
    }
    return TOK_SEMICOLON;
}
{COMMA} {
    if(scan_only == 1) {
        printf("<COMMA, %s>\n", yytext);
        return 1;
    }
    return TOK_COMMA;
}
{ASSIGNOP} {
    if(scan_only == 1) {
        printf("<ASSIGNOP, %s>\n", yytext);
        return 1;
    }
    yylval.strval = strdup(yytext);
    return TOK_ASSIGNOP;
}
{PLUSOP} {
    if(scan_only == 1) {
        printf("<PLUSOP, %s>\n", yytext);
        return 1;
    }
    yylval.strval = strdup(yytext);
    return TOK_PLUSOP;
}
{MINUSOP} {if(scan_only == 1) {
        printf("<MINUSOP, %s>\n", yytext);
        return 1;
    }
    yylval.strval = strdup(yytext);
    return TOK_MINUSOP;
}
{ID} {
    if(scan_only == 1) {
        printf("<ID, %s>\n", yytext);\
        return 1;
    }
    yylval.strval = strdup(yytext);
    return TOK_ID;
}
{INTLITERAL} {
    if(scan_only == 1) {
        printf("<INTLITERAL, %s>\n", yytext);
        return 1;
    }
    yylval.intval = std::atoi(yytext);
    return TOK_INTLITERAL;
}
<<EOF>> {
    if(scan_only == 1) {
        printf("<SCANEOF>\n");
        // stop the while loop 
        return 0;
    }
    return TOK_SCANEOF;
}


%%

int yywrap(void) {return 1;}
