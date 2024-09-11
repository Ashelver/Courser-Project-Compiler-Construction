/**
 * --------------------------------------
 * CUHK-SZ CSC4180: Compiler Construction
 * Assignment 3: Oat v.1 Parser
 * --------------------------------------
 * Author: HaoLUO
 * Date: March 28th, 2024
 * 
 * File: parser.hpp
 * -----------------------------
 * 
 */


#ifndef PARSER_HPP
#define PARSER_HPP

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <unordered_map>
#include <stack>

const std::string EPSILON = "";

void printStackInLine(std::stack<std::string>& stringStack);
void printProductionInLine(const std::string &nonTerminal, const std::vector<std::string> &production);

class PredictiveParser {
public:
    PredictiveParser();

    bool scan(std::string &filename);

    void addProduction(const std::string &nonTerminal, const std::vector<std::string> &production);
    void addProduction_start(const std::string &nonTerminal, const std::vector<std::string> &production);

    void DeBug();

    void buildParsingTable();

    void parsing();

private:
    // Data Structures
    std::string startSymbol;
    std::vector<std::string> tokens; // Input tokens
    std::unordered_map<std::string, std::vector<std::vector<std::string>>> grammar; // Grammars
    std::set<std::string> nonTerminalSet; // The Set of nonterminals
    std::unordered_map<std::string, std::set<std::string>> allFirstSets; // First sets
    std::unordered_map<std::string, std::set<std::string>> allFollowSets; // Follow sets
    std::unordered_map<std::string, std::unordered_map<std::string, std::vector<std::string>>> parsingTable; // The parsing table

    // Private Methods
    std::vector<std::string> tokenize(const std::string &source_code);
    bool isTerminal(const std::string &Input_Token);
    void calculateFirstSet();
    void calculateFollowSet();
};


#endif // PARSER_HPP