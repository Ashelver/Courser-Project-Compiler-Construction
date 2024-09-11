/**
 * --------------------------------------
 * CUHK-SZ CSC4180: Compiler Construction
 * Assignment 2: Oat v.1 Scanner
 * --------------------------------------
 * Author: HaoLUO
 * Date: March 16th, 2024
 * 
 * File: lexer.l
 * -----------------------------
 * This file implements some lexical analysis rules to scan the input file 
 * and return the tokens which will be used later for parser
 */


%{
/* Change this part if want to create parser */
#define TOK_UNIVERSAL 0
%}


%{
/* C/C++ Stuff (headers, declarations, variables, etc.) */
#include <cstdio>
#include <string>
#include <vector>

int yyerror (char *s);

int yylex();
%}

SPACES              (\t|\0|\r|\n|\ )+
NUL                 "null"
TRUE                "true"
FALSE               "false"
TVOID               "void"
TINT                "int"
TSTRING             "string"
TBOOL               "bool"
IF                  "if"
ELSE                "else"
WHILE               "while"
FOR                 "for"
RETURN              "return" 
NEW                 "new"
VAR                 "var"
GLOBAL              "global"
LPAREN              "("
RPAREN              ")"
LBRACE              "{"
RBRACE              "}"
LBRACKET            "["
RBRACKET            "]"
NOT                 "!"
TILDE               "~"
ASSIGN              "="
SEMICOLON           ";"
COMMA               ","
STAR                "*"
PLUS                "+"
MINUS               "-"
LSHIFT              "<<"
RLSHIFT             ">>"
RASHIFT             ">>>"
LESS                "<"
LESSEQ              "<="
GREAT               ">"
GREATEQ             ">="
EQ                  "=="
NEQ                 "!="
LAND                "&"
LOR                 "|"
BAND                "[&]"
BOR                 "[|]"
INTEGER             [0-9]+
ID                  [a-zA-Z][a-zA-Z0-9_]*
COMMENT             \/\*([^*]|\*+[^*/])*\*+\/
STRING              \"(.)*\"


%%

{SPACES} { /* Skip */ }
{COMMENT} { /* Skip */ }
{NUL} {
  printf("NUL %s\n",yytext);
}
{TRUE} {
  printf("TRUE %s\n",yytext);
}
{FALSE} {
  printf("FALSE %s\n",yytext);
}
{TVOID} {
  printf("TVOID %s\n",yytext);
}
{TINT} {
  printf("TINT %s\n",yytext);
}
{TSTRING} {
  printf("TSTRING %s\n",yytext);
}
{TBOOL} {
  printf("TBOOL %s\n",yytext);
}
{IF} {
  printf("IF %s\n",yytext);
}
{ELSE} {
  printf("ELSE %s\n",yytext);
}
{WHILE} {
  printf("WHILE %s\n",yytext);
}
{FOR} {
  printf("FOR %s\n",yytext);
}
{RETURN} {
  printf("RETURN %s\n",yytext);
}
{NEW} {
  printf("NEW %s\n",yytext);
}
{VAR} {
  printf("VAR %s\n",yytext);
}
{GLOBAL} {
  printf("GLOBAL %s\n",yytext);
}
{LPAREN} {
  printf("LPAREN %s\n",yytext);
}
{RPAREN} {
  printf("RPAREN %s\n", yytext);
}
{LBRACE} {
  printf("LBRACE %s\n",yytext);
}
{RBRACE} {
  printf("RBRACE %s\n",yytext);
}
{LBRACKET} {
  printf("LBRACKET %s\n",yytext);
}
{RBRACKET} {
  printf("RBRACKET %s\n",yytext);
}
{NOT} {
  printf("NOT %s\n",yytext);
}
{TILDE} {
  printf("TILDE %s\n",yytext);
}
{ASSIGN} {
  printf("ASSIGN %s\n",yytext);
}
{SEMICOLON} {
  printf("SEMICOLON %s\n",yytext);
}
{COMMA} {
  printf("COMMA %s\n",yytext);
}
{STAR} {
  printf("STAR %s\n",yytext);
}
{PLUS} {
  printf("PLUS %s\n",yytext);
}
{MINUS} {
  printf("MINUS %s\n",yytext);
}
{LSHIFT} {
  printf("LSHIFT %s\n",yytext);
}
{RLSHIFT} {
  printf("RLSHIFT %s\n",yytext);
}
{RASHIFT} {
  printf("RASHIFT %s\n",yytext);
}
{LESS} {
  printf("LESS %s\n",yytext);
}
{LESSEQ} {
  printf("LESSEQ %s\n",yytext);
}
{GREAT} {
  printf("GREAT %s\n",yytext);
}
{GREATEQ} {
  printf("GREATEQ %s\n",yytext);
}
{EQ} {
  printf("EQ %s\n",yytext);
}
{NEQ} {
  printf("NEQ %s\n",yytext);
}
{LAND} {
  printf("LAND %s\n",yytext);
}
{LOR} {
  printf("LOR %s\n",yytext);
}
{BAND} {
  printf("BAND %s\n",yytext);
}
{BOR} {
  printf("BOR %s\n",yytext);
}
{STRING} {
  printf("STRINGLITERAL %s\n",yytext);
}
{INTEGER} {
  printf("INTLITERAL %s\n",yytext);
}
{ID} {
  printf("ID %s\n",yytext);
}
<<EOF>> {
  return TOK_UNIVERSAL;
}




%%

int main(int argc, char **argv)
{
  yylex();
  yywrap();
}

int yywrap (void) {return 1;}