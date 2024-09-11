/**
 * --------------------------------------
 * CUHK-SZ CSC4180: Compiler Construction
 * Assignment 3: Oat v.1 Parser
 * --------------------------------------
 * Author: HaoLUO
 * Date: March 28th, 2024
 * 
 * File: main.cpp
 * -----------------------------
 * This file asks the user to input a file name and generates its ast tree
 */

#include "parser.hpp"

int main(int argc, char const *argv[]) {
    if (argc == 2) {
        std::string filename = argv[1];
        PredictiveParser parser;
        // strat Nonterminal
        parser.addProduction_start("S", {"prog", "$"});
        // prog
        parser.addProduction("prog", {"decl", "prog"});
        parser.addProduction("prog", {EPSILON});
        // decl
        parser.addProduction("decl",{"gdecl"});
        parser.addProduction("decl",{"fdecl"});
        // gdecl
        parser.addProduction("gdecl",{"global", "id" , "=", "gexp", ";"});
        // fdecl
        parser.addProduction("fdecl",{"t", "id", "(", "args", ")", "block"});
        // args
        parser.addProduction("args",{"arg", "args_"});
        parser.addProduction("args",{EPSILON});
        // args_
        parser.addProduction("args_",{",", "arg", "args_"});
        parser.addProduction("args_",{EPSILON});
        // arg
        parser.addProduction("arg",{"t","id"});
        // block
        parser.addProduction("block",{"{", "stmts", "}"});
        
        // t 
        parser.addProduction("t",{"primary_t", "t_arr"});
        // t_arr
        parser.addProduction("t_arr",{"[", "]"});
        parser.addProduction("t_arr",{EPSILON});
        // primary_t
        parser.addProduction("primary_t",{"int"});
        parser.addProduction("primary_t",{"bool"});
        parser.addProduction("primary_t",{"string"});
        // gexps
        parser.addProduction("gexps",{"gexp", "gexps_"});
        parser.addProduction("gexps",{EPSILON});
        // gexps_
        parser.addProduction("gexps_",{",", "gexp", "gexps_"});
        parser.addProduction("gexps_",{EPSILON});
        // gexp
        parser.addProduction("gexp",{"intliteral"});
        parser.addProduction("gexp",{"stringliteral"});
        parser.addProduction("gexp",{"t", "null"});
        parser.addProduction("gexp",{"true"});
        parser.addProduction("gexp",{"false"});
        parser.addProduction("gexp",{"new", "t", "{", "gexps", "}"});
        // stmts
        parser.addProduction("stmts",{"stmt", "stmts"});
        parser.addProduction("stmts",{EPSILON});
        // stmt
        parser.addProduction("stmt",{"id", "stmt_"});
        // stmt_
        parser.addProduction("stmt_",{"func_call", "arr_idx", "assign", ";"});
        // func_call
        parser.addProduction("func_call",{EPSILON});
        parser.addProduction("func_call",{"(", "exps", ")"});
        // arr_idx
        parser.addProduction("arr_idx",{EPSILON});
        parser.addProduction("arr_idx",{"[", "exp", "]"});
        // assign
        parser.addProduction("assign",{"=", "exp"});
        parser.addProduction("assign",{EPSILON});
        // stmt
        parser.addProduction("stmt",{"vdecl", ";"});
        parser.addProduction("stmt",{"return", "exp", ";"});
        parser.addProduction("stmt",{"if_stmt"});
        parser.addProduction("stmt",{"for", "(", "vdecls", ";", "exp_opt", ";", "stmt_opt", ")", "block"});
        parser.addProduction("stmt",{"while", "(", "exp", ")", "block"});
        // stmt_opt
        parser.addProduction("stmt_opt",{"stmt"});
        parser.addProduction("stmt_opt",{EPSILON});

        // if_stmt
        parser.addProduction("if_stmt",{"if", "(", "exp", ")", "block", "else_stmt"});
        // else_stmt
        parser.addProduction("else_stmt",{"else", "else_body"});
        parser.addProduction("else_stmt",{EPSILON});
        // else_body
        parser.addProduction("else_body",{"block"});
        parser.addProduction("else_body",{"if_stmt"});
        // exp_opt
        parser.addProduction("exp_opt",{"exp"});
        parser.addProduction("exp_opt",{EPSILON});
        // vdecls
        parser.addProduction("vdecls",{"vdecl", "vdecls"});
        parser.addProduction("vdecl vdecls",{EPSILON});
        // vdecl
        parser.addProduction("vdecl",{"var", "id", "=", "exp"});
        
        // exps
        parser.addProduction("exps",{"exp", "exps_"});
        // exps_
        parser.addProduction("exps_",{",", "exp", "exps_"});
        parser.addProduction("exps_",{EPSILON});
        // exp
        parser.addProduction("exp",{"term", "exp_"});
        // exp_
        parser.addProduction("exp_",{"bop", "term", "exp_"});
        parser.addProduction("exp_",{EPSILON});
        // term 
        parser.addProduction("term",{"primary"});
        parser.addProduction("term",{"uop", "primary"});
        // primary
        parser.addProduction("primary",{"id", "func_call", "arr_idx"});
        parser.addProduction("primary",{"intliteral"});
        parser.addProduction("primary",{"stringliteral"});
        parser.addProduction("primary",{"t", "null"});
        parser.addProduction("primary",{"true"});
        parser.addProduction("primary",{"false"});
        parser.addProduction("primary",{"(", "exp", ")"});
        // bop
        parser.addProduction("bop",{"*"});
        parser.addProduction("bop",{"+"});
        parser.addProduction("bop",{"-"});
        parser.addProduction("bop",{"<<"});
        parser.addProduction("bop",{">>"});
        parser.addProduction("bop",{">>>"});
        parser.addProduction("bop",{"<"});
        parser.addProduction("bop",{"<="});
        parser.addProduction("bop",{">"});
        parser.addProduction("bop",{">="});
        parser.addProduction("bop",{"=="});
        parser.addProduction("bop",{"!="});
        parser.addProduction("bop",{"&"});
        parser.addProduction("bop",{"|"});
        parser.addProduction("bop",{"[&]"});
        parser.addProduction("bop",{"[|]"});
        // uop
        parser.addProduction("uop",{"-"});
        parser.addProduction("uop",{"!"});
        parser.addProduction("uop",{"~"});
        parser.buildParsingTable();
        if (parser.scan(filename)) parser.parsing();
        // parser.DeBug();
    } else {
        std::cout << "Please input the file name of Oat v.1 source program." << std::endl;
    }
    return 0;
}
