/**
 * --------------------------------------
 * CUHK-SZ CSC4180: Compiler Construction
 * Assignment 3: Oat v.1 Parser
 * --------------------------------------
 * Author: HaoLUO
 * Date: March 28th, 2024
 * 
 * File: parser.cpp
 * -----------------------------
 * 
 */


#include "parser.hpp"

// Tool functions:
void printStackInLine(std::stack<std::string>& stringStack) {
    // Save the elements into a vector
    std::vector<std::string> tempVector;
    std::stack<std::string> tempStack = stringStack;
    while (!tempStack.empty()) {
        tempVector.push_back(tempStack.top());
        tempStack.pop();
    }

    std::cout << "           Stack: ";
    // Print the elements in reverse order
    for (auto it = tempVector.rbegin(); it != tempVector.rend(); ++it) {
        std::cout << *it;
        if (std::next(it) != tempVector.rend()) {
            std::cout << " ";
        }
    }
    std::cout << std::endl;
}

void printProductionInLine(const std::string &nonTerminal, const std::vector<std::string> &production) {
    std::cout << "            Rule: " << nonTerminal << " --> ";
    
    // Printf the string in vector in order
    for (size_t i = 0; i < production.size(); ++i) {
        if (production[i] == EPSILON) {
            std::cout << "ε";
        } else {
            std::cout << production[i];
        }
        if (i != production.size() - 1) {
            std::cout << " ";
        }
    }

    std::cout << std::endl;
}


// PredictiveParser
PredictiveParser::PredictiveParser() {
    // 
}

bool PredictiveParser::scan(std::string &filename) {
    // open source code
    std::ifstream file(filename);
    if (!file.is_open()) {
        // open failed
        std::cerr << "Cannot find the file!" << std::endl;
        return false;
    }

    // read file into stream buffer
    std::string source_code((std::istreambuf_iterator<char>(file)),
                             std::istreambuf_iterator<char>());

    // tokenize the source code
    std::vector<std::string> tokens = tokenize(source_code);

    // // print the tokens
    // for (const std::string &token : tokens) {
    //     std::cout << token << std::endl;
    // }

    // close the file
    file.close();

    return true;
}

// Process the input tokens
std::vector<std::string> PredictiveParser::tokenize(const std::string &source_code) {
    std::string token;
    size_t start = 0;
    size_t end = source_code.find(' ');

    while (end != std::string::npos) {
        token = source_code.substr(start, end - start);
        tokens.push_back(token);
        start = end + 1;
        end = source_code.find(' ', start);
    }

    // Push the last token
    token = source_code.substr(start);
    tokens.push_back(token);

    return tokens;
}

// Check a token is a terminal symbol or not 
bool PredictiveParser::isTerminal(const std::string &Input_Token) {
    if (nonTerminalSet.find(Input_Token) != nonTerminalSet.end()) return false;
    return true;
}

// Add strat symbol into the grammar
void PredictiveParser::addProduction_start(const std::string &nonTerminal, const std::vector<std::string> &production) {
    grammar[nonTerminal].push_back(production);
    nonTerminalSet.insert(nonTerminal);
    startSymbol = nonTerminal;
}

// Add a production
void PredictiveParser::addProduction(const std::string &nonTerminal, const std::vector<std::string> &production) {
    grammar[nonTerminal].push_back(production);
    nonTerminalSet.insert(nonTerminal);
}


void PredictiveParser::DeBug() {
    // for (const auto& pair : grammar) {
    //     std::cout << "Key: " << pair.first << std::endl;
    //     const std::vector<std::vector<std::string>>& vec_vec = pair.second;
    //     for (const auto& vec : vec_vec) {
    //         for (const std::string& str : vec) {
    //             std::cout << "Value: " << str << std::endl;
    //         }
    //     }
    // }

    // for (const auto& token : tokens) {
    //     if (!isTerminal(token)) printf("Wrong");
    // }

    // std::cout << "Start Symbol: " << startSymbol << std::endl;

    // std::cout << "First Set" << std::endl;
    // for (const auto& pair : allFirstSets) {
    //     std::cout << "Nonterminal: "<< pair.first << ", Fisrt:";
    //     for (const auto& it : pair.second) {
    //         std::cout << it << " ";
    //     }
    //     std::cout << std::endl;
    // }


    // std::cout << "\n" << "Follow Set" << std::endl;
    // for (const auto& pair : allFollowSets) {
    //     std::cout << "Nonterminal: "<< pair.first << ", Follow:";
    //     for (const auto& it : pair.second) {
    //         std::cout << it << " ";
    //     }
    //     std::cout << std::endl;
    // }

    // // parsingTable
    // std::cout << "Parsing Table:" << std::endl;
    // for (const auto& stateEntry : parsingTable) {
    //     std::cout << "State: " << stateEntry.first << std::endl;
    //     for (const auto& symbolEntry : stateEntry.second) {
    //         std::cout << "  Symbol: " << symbolEntry.first << std::endl;
    //         std::cout << "    Actions: ";
    //         for (const auto& action : symbolEntry.second) {
    //             std::cout << action << " ";
    //         }
    //         std::cout << std::endl;
    //     }
    // }


    std::cout << tokens.size();
}

// build the parsing table
void PredictiveParser::buildParsingTable() {
    // Construct FIRST and FOLLOW sets
    calculateFirstSet();
    calculateFollowSet();

    // Go through every production set 
    for (const auto& pair : grammar) {
        const std::string& nonTerminal = pair.first;
        const std::vector<std::vector<std::string>>& productions = pair.second;
        size_t size = productions.size();
        for (const auto& production : productions) {
            size --;
            // Calculate the production FIRST set
            std::set<std::string> firstSet;
            for (const auto& symbol : production) {
                if (isTerminal(symbol)) {
                    firstSet.insert(symbol);
                    break;
                } else {
                    for (const auto& terminal : allFirstSets[symbol]) {
                        if ((terminal != EPSILON) || (size == 0)) firstSet.insert(terminal);
                    }
                    if (allFirstSets[symbol].find(EPSILON) == allFirstSets[symbol].end()) {
                        // EPSILON is not in the first set of this nonterminal
                        break;
                    }
                }
            }

            // For every terminal in FIRST(production) 
            for (const auto& terminal : firstSet) {
                // If terminal is not ε, then fill the production formula into the corresponding table cells
                if (terminal != EPSILON) {
                    if (!parsingTable[nonTerminal][terminal].empty()) std::cerr << "Conflict detected for non-terminal " << nonTerminal << " and terminal " << terminal << std::endl;
                    parsingTable[nonTerminal][terminal] = production;
                } else {
                    // Otherwise, the production will be filled into the table cells corresponding to each terminator in FOLLOW (non Terminal)
                    for (const auto& followTerminal : allFollowSets[nonTerminal]) {
                        if (!parsingTable[nonTerminal][followTerminal].empty()) std::cerr << "Conflict detected for non-terminal " << nonTerminal << " and terminal " << terminal << std::endl;
                        parsingTable[nonTerminal][followTerminal] = production;
                    }
                }
            }
        }
    }
}

// Implement calculation of FIRST set
void PredictiveParser::calculateFirstSet() {
    // Initialize the first set
    for (const auto& rule : grammar) {
        allFirstSets[rule.first] = std::set<std::string>();
    }


    bool updated;
    do {
        updated = false;
        // Calculate the First Set of all the NonTerminals
        for (const auto& rule : grammar) {
            const std::string& nonTerminal = rule.first;

            std::set<std::string>& firstSet = allFirstSets[nonTerminal];
            for (const auto& production : grammar.at(nonTerminal)) {
                size_t i = 0;
                while (i < production.size()) {
                    const std::string& symbol = production[i];
                    if (symbol == EPSILON) {
                        if (firstSet.find(EPSILON) == firstSet.end()) {
                            firstSet.insert(EPSILON);
                            updated = true;
                        }
                    } else if (isTerminal(symbol)) { // Terminal symbol
                        if (firstSet.find(symbol) == firstSet.end()) {
                            firstSet.insert(symbol);
                            updated = true;
                        }
                        break;
                    } else { // Non-terminal symbol
                        const std::set<std::string>& symbolFirstSet = allFirstSets[symbol];
                        for (const std::string& terminal : symbolFirstSet) {
                            if (terminal != EPSILON) {
                                if (firstSet.find(terminal) == firstSet.end()) {
                                    firstSet.insert(terminal);
                                    updated = true;
                                }
                            }
                        }
                        // Continue to the next symbol if symbol can derive ε
                        if (symbolFirstSet.find(EPSILON) == symbolFirstSet.end()) {
                            break;
                        }
                    }
                    ++i;
                }
                // If all symbols in the production can derive ε, add ε to the current non-terminal's FIRST set
                if (i == production.size()) {
                    if (firstSet.find(EPSILON) == firstSet.end()) {
                        firstSet.insert(EPSILON);
                        updated = true;
                    }
                }
            }
        }
    } while (updated);
}


// Implement calculation of FOLLOW set
void PredictiveParser::calculateFollowSet() {
    // Initialize FOLLOW sets
    for (const auto& rule : grammar) {
        allFollowSets[rule.first] = std::set<std::string>();
    }
    // allFollowSets[startSymbol].insert("$");

    bool updated;
    do {
        updated = false;
        for (const auto& rule : grammar) {
            const std::string& nonTerminal = rule.first;

            for (const auto& production : rule.second) {
                for (size_t i = 0; i < production.size(); ++i) {
                    const std::string& symbol = production[i];
                    if (isTerminal(symbol)) {
                        continue;
                    }
                    
                    if (i == production.size() - 1) {
                        // The last symbol is nonterminal, add the non finalizer FOLLOW set on the left side of the production to the FOLLOW set of A
                        for (auto& terminal : allFollowSets[nonTerminal]) {
                            if (allFollowSets[symbol].insert(terminal).second) {
                                updated = true;
                            }
                        }
                        continue;
                    }

                    // The inside symbols is nonterminal
                    const std::vector<std::string>& restSymbols = std::vector<std::string>(production.begin() + i + 1, production.end());
                    for (const std::string& restsymbol : restSymbols) {
                        if (isTerminal(restsymbol)) {
                            // Nonterminal, directly add it into followset
                            if (allFollowSets[symbol].insert(restsymbol).second) {
                                updated = true;
                            }
                            break;
                        }
                        if (restsymbol ==  restSymbols.back()) {
                            // The last symbol is nonterminal, add the non finalizer FOLLOW set on the left side of the production to the FOLLOW set of A
                            for (auto& terminal : allFollowSets[nonTerminal]) {
                                if (allFollowSets[symbol].insert(terminal).second) {
                                    updated = true;
                                }
                            }
                        }
                        if (allFirstSets[restsymbol].find(EPSILON) == allFirstSets[restsymbol].end()) {
                            // no epsilon in the first set of this symbol
                            for (auto& terminal : allFirstSets[restsymbol]) {
                                if (allFollowSets[symbol].insert(terminal).second) {
                                    updated = true;
                                }
                            }                         
                            break;                     
                        } else {
                            // epsilon in the first set of this symbol 
                            for (auto& terminal : allFirstSets[restsymbol]) {
                                if (terminal != EPSILON) {
                                    if (allFollowSets[symbol].insert(terminal).second) {
                                        updated = true;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    } while (updated);
}


// Parsing
void PredictiveParser::parsing() {
    /*
    The input tokens are in the `tokens`, wichi is a vector<string>
    The parsing table is parsingTable,
    parsingTable[state][symbol] will return a vector of right hand side production as a vector<string>
    */
    std::stack<std::string> symbols_stack;
    symbols_stack.push("$"); // Means the end of the scanning
    symbols_stack.push("prog"); // The start of the program
    size_t index = 0; // The index of the token

    std::string processed_tokens = "";

    std::cout << "Start Parsing:\n" << std::endl;
    // Start parsing
    while (!symbols_stack.empty()) {
        // std::string input;
        // std::cout << "Please enter something (press Enter to continue): " << std::endl;
        // std::getline(std::cin, input);
    
        std::string current_state = symbols_stack.top();
        std::string current_token;
        if (index == tokens.size()) {
            current_token = "$";
        } else {
            current_token = tokens[index];
        }
        // Printf the information
        printStackInLine(symbols_stack);
        std::cout << "Processed Inputs:" << processed_tokens << std::endl;
        std::cout << "   Current Input: " << current_token << std::endl;

        if (isTerminal(current_state)) {
            if (current_state == current_token) {
                symbols_stack.pop();
                processed_tokens += " ";
                processed_tokens += current_token;
                index ++;
                std::cout << "            Rule: " << "Match " << current_token << "\n\n" << std::endl;
            } else {
                std::cerr << "Cannot match the token with the state, Error!" << std::endl;
            }
        } else {
            std::vector<std::string> Rule = parsingTable[current_state][current_token];
            if (Rule.empty()){
                // No next action found! Print out Error
                std::cerr << "Can not find the next action, Error!" << std::endl;
            }
            printProductionInLine(current_state, Rule);
            std::cout << "\n" << std::endl;
            symbols_stack.pop();
            if (Rule[0] == EPSILON) {
                // empty derivation
                continue;
            }
            for (auto it = Rule.rbegin(); it != Rule.rend(); ++it) {
                symbols_stack.push(*it);
            }
        }
    }
    std::cout << "Accept!" << std::endl;
}


