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

#ifndef CSC4180_IR_GENERATOR_HPP
#define CSC4180_IR_GENERATOR_HPP

#include "node.hpp"

/**
 * LLVM IR Generator of Micro Language
 * It takes the AST generated from parser and generate LLVM IR instructions.
 */
class IR_Generator {
public:
    IR_Generator(std::ofstream &output)
        : out(output) {}

    /**
     * Export AST to LLVM IR file
     * 
     * It calls gen_llvm_ir recursively to generate LLVM IR instruction for each node in the AST
     * 
     * @param node
     * @return
     */
    void export_ast_to_llvm_ir(Node* node);

private:
    /**
     * Recursively generate LLVM IR of the given AST tree node
     * 
     * Should have different logic for different symbol classes
     * 
     * this should be a recursive function
     */
    void gen_llvm_ir(Node* node);

    void gen_read_llvm_ir(Node* node);

    void gen_write_llvm_ir(Node* node);

    std::string find_tmp_register();

    std::string reference(Node* node);

    std::string combine(Node* node);

    void gen_assignop_llvm_ir(Node* node);

private:
    std::ofstream &out;
};

#endif  // CSC4180_IR_GENERATOR_HPP
