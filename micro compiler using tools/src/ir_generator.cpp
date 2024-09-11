/**
 * --------------------------------------
 * CUHK-SZ CSC4180: Compiler Construction
 * Assignment 1: Micro Language Compiler
 * --------------------------------------
 * Author: HaoLUO
 * Date: February 27th, 2024
 * 
 * This file defines the LLVM IR Generator class, which generate LLVM IR (.ll) file given the AST from parser.
 */

#include "ir_generator.hpp"

std::vector<std::string> ID_TABLE;
std::vector<int> tmp_register;

void IR_Generator::export_ast_to_llvm_ir(Node* node) {
    if (node == nullptr) {
        std::cerr << "Error: the ast is empty!" << std::endl;
        return;
    }
    // Manually set
    out << "; Declare printf";
    out << std::endl;
    out << "declare i32 @printf(i8*, ...)";
    out << std::endl;
    out << std::endl;
    out << "; Declare scanf";
    out << std::endl;
    out << "declare i32 @scanf(i8*, ...)";
    out << std::endl;
    out << std::endl;
    out << "define i32 @main() {";
    out << std::endl;

    gen_llvm_ir(node);

    out << "\tret i32 0";
    out << std::endl;
    out << "}";
    out << std::endl;

    out.close();
}

void IR_Generator::gen_llvm_ir(Node* node) {
    if (node == nullptr) {
        std::cerr << "Error: AST node is empty" << std::endl;
        return;
    }
    // Seperated functions for different situation
    /*
    Skip: ID, INTLITERAL
    Special: ASSIGNOP, READ, WRITE, 
    Recursive: Others
    */
    switch (node->symbol_class) {
        case SymbolClass::ASSIGNOP:
            gen_assignop_llvm_ir(node);
            break;
        case SymbolClass::READ:
            gen_read_llvm_ir(node);
            break;
        case SymbolClass::WRITE:
            gen_write_llvm_ir(node);
            break;
        case SymbolClass::ID:
        case SymbolClass::INTLITERAL:
            break;
        default:
            // Recursively generate LLVM IR for each child node
            for (auto &child : node->children) {
                gen_llvm_ir(child);
            }
            break;
    }
    return;
}


/*
 * %<variable> = alloca i32
 * %_scanf_format_1 = alloca [# x i8]
 * store [# x i8] c"%d ... %d\00", [# x i8]* %_scanf_format_1
 * %_scanf_str_1 = getelementptr [# x i8], [# x i8]* %_scanf_format_1, i32 0, i32 0
 * call i32 (i8*, ...) @scanf(i8* %_sacnf_str_1, i32* %<variable>)
 */
void IR_Generator::gen_read_llvm_ir(Node* node) {
    std::string variable_list = "";
    std::string format_info = "";
    for (int i = 0; i < node->children.size(); i++) {
        std::string variable = node->children[i]->lexeme;
        bool flag = 1;
        for (int j = 0; j < ID_TABLE.size(); j++){
            if (variable == ID_TABLE[j]){
                flag = 0;
                break;
            }
        }
        if (flag){
            ID_TABLE.push_back(variable);
            out << "\t%" << variable << " = alloca i32" << std::endl;
            variable_list += ", i32* %" + variable;
        }
        format_info += "%d ";
    }
    format_info = std::string(format_info.begin(), format_info.end() - 1);

    int i8_num = format_info.length() + 1;
    std::string i8_string = std::to_string(i8_num) + " x i8";

    out << "\t%_scanf_format_1 = alloca [" << i8_string << "]" << std::endl;
    out << "\tstore [" << i8_string << "] c\"" << format_info << "\\00\", [" << i8_string << "]* %_scanf_format_1" << std::endl;
    out << "\t%_scanf_str_1 = getelementptr [" << i8_string << "], [" << i8_string << "]* %_scanf_format_1, i32 0, i32 0" << std::endl;
    out << "\tcall i32 (i8*, ...) @scanf(i8* %_scanf_str_1" << variable_list << ")" << std::endl;
}


/* 
 * %_printf_format_1 = alloca [# x i8]
 * store [# x i8] c"%d ... %d\0A\00", [# x i8]* %_printf_format_1
 * %_printf_str_1 = getelementptr [# x i8], [# x i8]* %_printf_format_1, i32 0, i32 0
 * call i32 (i8*, ...) @printf(i8* %_printf_str_1, i32 <rvalue>)
 */
void IR_Generator::gen_write_llvm_ir(Node* node) {
    int variable_num = node->children.size();

    std::string format_info = "";
    for (int i = 0; i < variable_num; i++) {
        format_info += "%d ";
    }
    format_info = std::string(format_info.begin(), format_info.end() - 1);

    int i8_num = format_info.length() + 2;
    std::string i8_string = std::to_string(i8_num) + " x i8";

    out << "\t%_printf_format_1 = alloca [" << i8_string << "]" << std::endl;
    out << "\tstore [" << i8_string << "] c\"" << format_info << "\\0A\\00\", [" << i8_string << "]* %_printf_format_1" << std::endl;
    out << "\t%_printf_str_1 = getelementptr [" << i8_string << "], [" << i8_string << "]* %_printf_format_1, i32 0, i32 0" << std::endl;
    
    std::string variable_list = "";
    for (int i = 0; i < node->children.size(); i++) {
        switch (node->children[i]->symbol_class) {
            case SymbolClass::ID:
            case SymbolClass::INTLITERAL:
                variable_list += ", i32 " + reference(node->children[i]);
                break;
            case SymbolClass::PLUSOP:
            case SymbolClass::MINUSOP:
                variable_list += ", i32 " + combine(node->children[i]);
                break;
            default:{
                std::cerr << "Invalid operand!" << std::endl;
                break;
            }
        }
    }
    out << "\tcall i32 (i8*, ...) @printf(i8* %_printf_str_1" << variable_list << ")" << std::endl;
}


// Get the available register
std::string IR_Generator::find_tmp_register(){
    int l = tmp_register.size(); 
    for (int i = 0; i < l; ++i){
        if (tmp_register[i] == 0){
            tmp_register[i] = 1;
            return std::to_string(i+1);
        }
    }
    tmp_register.push_back(1);
    return std::to_string(l+1);
}

// When an intger of variable is referenced
std::string IR_Generator::reference(Node* node){
    switch (node->symbol_class) {
        case SymbolClass::ID:{
            std::string temp_variable = "%_tmp_" + find_tmp_register();
            out << "\t" << temp_variable << " = load i32, i32* %" << node->lexeme << std::endl;
            return temp_variable;
            break;
        }
        case SymbolClass::INTLITERAL:{
            return node->lexeme;
            break;
        }
        default:
            std::cerr << "Invalid type of reference" << std::endl;
            break;
    }
    return "";
}

// Combine the varibles and operations, left child-node-right child
std::string IR_Generator::combine(Node* node){
    std::string operation = "";
    switch (node->symbol_class){
        case SymbolClass::PLUSOP:{
            operation = "add";
            break;
        }
        case SymbolClass::MINUSOP:{
            operation = "sub";
            break;
        }
        default:{
            std::cerr << "Error oprand!" << std::endl;
            break;
        }
    }
    std::string lvalue;
    std::string rvalue;
    Node* left_child = node->children[0];
    Node* right_child = node->children[1];
    switch (left_child->symbol_class){
        case SymbolClass::ID:{
            lvalue = reference(left_child);
            break;
        }
        case SymbolClass::INTLITERAL:{
            lvalue = reference(left_child);
            break;
        }
        case SymbolClass::PLUSOP:
        case SymbolClass::MINUSOP:
            lvalue = combine(left_child);
            break;
        default:{
            std::cerr << "Error oprand!" << std::endl;
            break;
        }
    }

    switch (right_child->symbol_class){
        case SymbolClass::ID:{
            rvalue = reference(right_child);
            break;
        }
        case SymbolClass::INTLITERAL:{
            rvalue = reference(right_child);
            break;
        }
        case SymbolClass::PLUSOP:
        case SymbolClass::MINUSOP:
            rvalue = combine(right_child);
            break;
        default:{
            std::cerr << "Error oprand!" << std::endl;
            break;
        }
    }
    std::string temp_variable = "%_tmp_" + find_tmp_register();
    out << "\t" << temp_variable << " = " << operation << " i32 " << lvalue << ", " << rvalue << std::endl;
    return temp_variable;
}

/*
 * ":=" operation, the left varible is children[0], the right value is children[1]
 * %* = alloca i32
 * store i32 value, i32* %*
 */
void IR_Generator::gen_assignop_llvm_ir(Node* node){
    std::string variable = node->children[0]->lexeme; // The variable name
    std::string right_value;

    switch (node->children[1]->symbol_class) {
        case SymbolClass::ID:
        case SymbolClass::INTLITERAL:
            right_value = reference(node->children[1]);
            break;
        case SymbolClass::PLUSOP:
        case SymbolClass::MINUSOP:
            right_value = combine(node->children[1]);
            break;
        default:{
            std::cerr << "Invalid symbol!" <<std::endl;
            break;
        }
    }
    // Check whether need to declear
    bool flag = 1;
    for (int j = 0; j < ID_TABLE.size(); j++){
        if (variable == ID_TABLE[j]){
            flag = 0;
            break;
        }
    }
    if (flag){
        // variable not in the table
        ID_TABLE.push_back(variable);
        out << "\t%" << variable << " = alloca i32" << std::endl;        
    }
    out << "\tstore i32 " << right_value << ", i32* %" << variable << std::endl;
}

