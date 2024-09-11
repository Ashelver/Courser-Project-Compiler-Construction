/**
 * --------------------------------------
 * CUHK-SZ CSC4180: Compiler Construction
 * Assignment 2: Oat v.1 Scanner
 * --------------------------------------
 * Author: HaoLUO
 * Date: March 17th, 2024
 * 
 * File: scanner.cpp
 * ------------------------------------------------------------
 * This file implements scanner function defined in scanner.hpp
 */

#include "scanner.hpp"

DFA::~DFA() {
    for (auto state : states) {
        state->transition.clear();
        delete state;
    }
}

void DFA::print() {
    printf("DFA:\n");
    for (auto state : states)
        state->print();
}

/**
 * Epsilon NFA
 * (Start) -[EPSILON]-> (End)
 */
NFA::NFA() {
    start = new State();
    end = new State();
    start->transition[EPSILON] = {end};
}

/**
 * NFA for a single character
 * (Start) -[c]-> (End)
 * It acts as the unit of operations like union, concat, and kleene star
 * @param c: a single char for NFA
 * @return NFA with only one char
 */
NFA::NFA(char c) {
    start = new State();
    end = new State();
    start->transition[c] = {end};
}

NFA::~NFA() {
    for (auto state : iter_states()) {
        state->transition.clear();
        delete state;
    }
}

/**
 * Concat a string of char
 * Start from the NFA of the first char, then merge all following char NFAs
 * @param str: input string
 * @return
 */
NFA* NFA::from_string(std::string str) {
    NFA* result = new NFA(str[0]);
    for (int i = 1; i < str.size(); ++i) {
        NFA* nfa_char = new NFA(str[i]);
        result->concat(nfa_char);
    }
    return result;
}

/**
 * RegExp: [a-zA-Z]
 * @return
 */
NFA* NFA::from_letter() {
    // Create start and end
    State* start = new State();
    State* end = new State();

    // Create transition
    for (char c = 'a'; c <= 'z'; ++c) {
        start->transition[c].insert(end);
    }
    for (char c = 'A'; c <= 'Z'; ++c) {
        start->transition[c].insert(end);
    }

    // Create nfa
    NFA* nfa = new NFA();
    nfa->start = start;
    nfa->end = end;

    return nfa;
}

/**
 * RegExp: [0-9]
 * @return
 */
NFA* NFA::from_digit() {
    // Create start and end
    State* start = new State();
    State* end = new State();

    // Create transition
    for (char c = '0'; c <= '9'; ++c) {
        start->transition[c].insert(end);
    }

    // Create nfa
    NFA* nfa = new NFA();
    nfa->start = start;
    nfa->end = end;

    return nfa;
}

/**
 * NFA for any char (ASCII 0-127)
 */
NFA* NFA::from_any_char() {
    // Create start and end
    State* start = new State();
    State* end = new State();

    // Create transition
    for (char c = 0; c < 127; ++c) {
        start->transition[c].insert(end);
    }

    // Create nfa
    NFA* nfa = new NFA();
    nfa->start = start;
    nfa->end = end;

    return nfa;
}



/**
 * Concatanat two NFAs
 * @param from: NFA pointer to be concated after the current NFA
 * @return: this -> from
 */
void NFA::concat(NFA* from) {
    // Ensure both NFAs are not null
    if (end != nullptr && from->start != nullptr) {
        // Connect the end state of the current NFA to the start state of the next NFA
        end->transition[EPSILON].insert(from->start);
        // Update the end state of the current NFA to be the end state of the next NFA
        end = from->end;
    }
}

/**
 * Set Union with another NFA
 * @param
 */
void NFA::set_union(NFA* from) {
    // Create a new start state
    State* new_start = new State();
    // Connect the new start state to the start states of both NFAs using epsilon transitions
    new_start->transition[EPSILON].insert(start);
    new_start->transition[EPSILON].insert(from->start);

    // Create a new end state
    State* new_end = new State();
    // Connect the end states of both NFAs to the new end state using epsilon transitions
    end->transition[EPSILON].insert(new_end);
    from->end->transition[EPSILON].insert(new_end);

    // Update the start and end pointers of the current NFA to point to the new start and end states
    start = new_start;
    end = new_end;
}

/**
 * Set Union with a set of NFAs
 */
void NFA::set_union(std::set<NFA*> set) {
    // Create a new start state
    State* new_start = new State();
    // Connect the new start state to the start states of all NFAs in the set using epsilon transitions
    for (auto nfa : set) {
        new_start->transition[EPSILON].insert(nfa->start);
    }

    // Create a new end state
    State* new_end = new State();
    // Connect the end states of all NFAs in the set to the new end state using epsilon transitions
    for (auto nfa : set) {
        nfa->end->transition[EPSILON].insert(new_end);
    }

    // Update the start and end pointers of the current NFA to point to the new start and end states
    start = new_start;
    end = new_end;
}

/**
 * Kleene Star Operation
 */
void NFA::kleene_star() {
    // Create a new start state
    State* new_start = new State();
    State* new_end = new State();
    // Connect the new start state to the start state of the current NFA and its end state using epsilon transitions
    new_start->transition[EPSILON].insert(start);
    new_start->transition[EPSILON].insert(new_end);

    // Connect the end state of the current NFA to the new start and end state of the current NFA using epsilon transitions
    end->transition[EPSILON].insert(new_start);
    end->transition[EPSILON].insert(new_end);

    // Update the start and end pointers of the current NFA to point to the new start and end states
    start = new_start;
    end = new_end;
}

/**
 * Determinize NFA to DFA by subset construction
 * @return DFA
 */
DFA* NFA::to_DFA() {
    // Create a DFA object
    DFA* dfa = new DFA();
    

    // Saving the processes states(NFA)
    std::map<std::set<State*>, DFA::State*> states_dfa_map;

    // Initializationn including ε-closure
    std::set<State*> initial_closure = epsilon_closure(start);
    auto init_state = new DFA::State();
    states_dfa_map[initial_closure] = init_state;
    dfa->states.push_back(init_state);

    // Using queue BFS
    std::queue<std::set<State*>> state_queue;
    state_queue.push(initial_closure);

    // Begin BFS
    while (!state_queue.empty()) {
        std::set<State*> current_closure = state_queue.front();
        state_queue.pop();

        // Go through every possible char (ASCII 128)
        for (char c = 0; c < 127; ++c) {
            // Get the closure if using char `c`
            std::set<State*> neighbor_states = move(current_closure, c);
            std::set<State*> next_closure;
            for (auto in_state : neighbor_states){
                std::set<State*> temp = epsilon_closure(in_state);
                next_closure.insert(temp.begin(),temp.end());
            }

            // If the arrivable next closure is not empty
            if (!neighbor_states.empty()) {
                // If this closure is not in the `states_dfa_map`
                if (states_dfa_map.find(next_closure) == states_dfa_map.end()) {
                    // Create and put a new DFA state int to the map
                    auto dfa_state= new DFA::State();
                    states_dfa_map[next_closure] = dfa_state;
                    dfa->states.push_back(dfa_state);
                    state_queue.push(next_closure);
                }
                // Build connection between current_closure and next_closure
                states_dfa_map[current_closure]->transition[c] = states_dfa_map[next_closure];
            }
        }
    }

    // Set the DFA `accepted` value
    for (auto &t : states_dfa_map) {
        for (auto nfa_state : t.first) {
            if (nfa_state->accepted) {
                t.second->accepted = true;
                t.second->token_class = nfa_state->token_class;
                break;
            }
        }
    }
    return dfa;
}

/**
 * Get the closure of the given Nstates set
 * It means all the Nstates can be reached with the given Nstates set without any actions
 * @param state: State* , the starting state of getting epsilon closure
 * @return {set<State&>} The closure of state
 */
std::set<NFA::State*> NFA::epsilon_closure(NFA::State* state) {
    std::set<NFA::State*> closure; // Set to store the closure of the given state
    std::queue<NFA::State*> queue; // Queue to perform breadth-first search for epsilon transitions


    // Start with the given state
    queue.push(state);

    // Perform breadth-first search to find all states reachable via epsilon transitions
    while (!queue.empty()) {
        NFA::State* current_state = queue.front();
        queue.pop();
        closure.insert(current_state); // Add the current state to the closure set

        // Traverse through all epsilon transitions from the current state
        auto epsilon_transitions = current_state->transition.find(EPSILON);
        if (epsilon_transitions != current_state->transition.end()) {
            for (auto next_state : epsilon_transitions->second) {
                // If the next state is not already in the closure set, push it onto the queue
                if (closure.find(next_state) == closure.end()) {
                    queue.push(next_state);
                }
            }
        }
    }
    return closure;
}

/**
 * Get the set of neighbor states from the closure of starting state through char c
 * @param closure
 * @param c
 * @return
 */
std::set<NFA::State*> NFA::move(std::set<NFA::State*> closure, char c) {
    // Set to store the neighbor states reachable from the closure through char c
    std::set<NFA::State*> next_states;

    // Iterate through each state in the closure
    for (auto state : closure) {
        // Check if there is a transition on char c from the current state
        auto transition = state->transition.find(c);
        if (transition != state->transition.end()) {
            // If there is a transition on char c, add all target states to the set of next states
            for (auto next_state : transition->second) {
                next_states.insert(next_state);
            }
        }
    }

    return next_states;
}

void NFA::print() {
    printf("NFA:\n");
    for (auto state : iter_states())
        state->print();
}

/**
 * BFS Traversal
 */
std::vector<NFA::State*> NFA::iter_states() {
    std::vector<State*> states{};
    states.emplace_back(start);
    std::queue<State*> states_to_go{};
    states_to_go.emplace(start);
    std::set<State*> visited_states = {start};
    while(!states_to_go.empty()) {
        State* state = states_to_go.front();
        states_to_go.pop();
        for (auto trans : state->transition) {
            for (auto neighbor : trans.second) {
                if (visited_states.find(neighbor) == visited_states.end()) {
                    states_to_go.emplace(neighbor);
                    visited_states.insert(neighbor);
                    states.emplace_back(neighbor);
                }
            }
        }
    }
    return states;
}

/**
 * Constructor: Scanner
 * @usage: Scanner origin;
 *         Scanner scanner(reserved_word_strs, token_strs, reserced_word_num, token_num);
 * --------------------------------------------------------------------------------------
 * Create a Scanner object. The default constructor will not be used, and the second form
 * will create the NFA and DFA machines based on the given reserved words and tokens
 */
Scanner::Scanner() {
    nfa = new NFA();
}

/**
 * Given a filename of a source program, print all the tokens of it
 * @param {string} filename
 * @return 0 for success, -1 for failure
 */ 
int Scanner::scan(std::string &filename) {
    // open source cdoe
    std::ifstream file(filename);
    if (!file.is_open()) {
        // open failed
        return -1;
    }

    // read file into stream buffer
    std::string source_code((std::istreambuf_iterator<char>(file)),
                             std::istreambuf_iterator<char>());

    auto start_dfa_state = dfa->states[0];
    auto pointer = start_dfa_state;
    std::string temp;
    // Go trough the file
    source_code += ' ';
    for (char c : source_code) {
        if (pointer == start_dfa_state){
            if (pointer->transition.find(c) == pointer->transition.end()){
                // Ignored
                continue;
            }
        }
        auto index = pointer->transition.find(c);
        // If it can continue
        if (index != pointer->transition.end()) {
            if (pointer->accepted && (pointer->token_class == STRINGLITERAL || pointer->token_class == COMMENT)){
                // Don't continue
            } else {
                temp += c;
                pointer = pointer->transition[c];
                continue;
            }
        }

        // Find the token class
        if (pointer->accepted){
            if (pointer->token_class != COMMENT) printf("%s %s\n",token_class_to_str(pointer->token_class).c_str(),temp.c_str());
            temp = "";
        } else {
            // Something wrong! 
            printf("Unkown %s\n",temp.c_str());
            temp = "";
        }
        
        pointer = start_dfa_state;
        index = pointer->transition.find(c);
        if (index != pointer->transition.end()) {
            temp += c;
            pointer = pointer->transition[c];
            continue;
        }
    }


    // close the file
    file.close();

    return 0;
}

/**
 * Add string tokens, usually for reserved words, punctuations, and operators
 * @param token_str: exact string to match for token recognition
 * @param token_class
 * @param precedence: precedence of token, especially for operators
 * @return
 */
void Scanner::add_token(std::string token_str, TokenClass token_class, unsigned int precedence) {
    auto keyword_nfa = NFA::from_string(token_str);
    keyword_nfa->set_token_class_for_end_state(token_class, precedence);
    nfa->set_union(keyword_nfa);
}

/**
 * Token: ID (Identifier)
 * RegExp: [a-zA-Z][a-zA-Z0-9_]*
 * @param token_class
 * @param precedence
 * @return
 */
void Scanner::add_identifier_token(TokenClass token_class, unsigned int precedence) {
    // start with letter
    NFA* identifier_nfa = NFA::from_letter();
    // following with Letter, digit, underscore 
    NFA* letter_digit_underscore_nfa = NFA::from_letter();
    letter_digit_underscore_nfa->set_union(NFA::from_digit());
    letter_digit_underscore_nfa->set_union(NFA::from_string("_"));

    letter_digit_underscore_nfa->kleene_star(); // repeat
    identifier_nfa->concat(letter_digit_underscore_nfa);

    // Set precedence
    identifier_nfa->set_token_class_for_end_state(token_class, precedence);

    // Union with primary nfa
    nfa->set_union(identifier_nfa);
}

/**
 * Token: INTEGER
 * RegExp: [1-9][0-9]+
 * Negative integer is recognized by unary operator MINUS
 * @param token_class
 * @param precedence
 * @return
 */
void Scanner::add_integer_token(TokenClass token_class, unsigned int precedence) {
    // //start with nonzero digit
    
    // following with unlimited digits
    NFA* integer_nfa = NFA::from_digit();
    NFA* following_nfa = NFA::from_digit();
    following_nfa->kleene_star(); //repeat
    integer_nfa->concat(following_nfa);

    // Set precedence
    integer_nfa->set_token_class_for_end_state(token_class, precedence);

    // Union with primary nfa
    nfa->set_union(integer_nfa);
}

/**
 * Token Class: STRINGLITERAL
 * RegExp: "(.|")*"
 * @param token_class
 * @param precedence
 * @return
 */
void Scanner::add_string_token(TokenClass token_class, unsigned int precedence) {
    // Start with "
    NFA* string_nfa = NFA::from_string("\"");

    // Any char
    NFA* any_char_nfa = NFA::from_any_char();
    any_char_nfa->set_union(NFA::from_string("\"")); 
    any_char_nfa->set_union(NFA::from_string("\\\""));
    any_char_nfa->kleene_star(); // repeat

    string_nfa->concat(any_char_nfa);

    // End with ""
    string_nfa->concat(NFA::from_string("\""));

    // Set precedence
    string_nfa->set_token_class_for_end_state(token_class, precedence);

    // Set precedence
    nfa->set_union(string_nfa);
}

/**
 * Token Class: COMMENT
 * RegExp: //\s(.)*
 * @param token_class
 * @param precedence
 * @return
 */
void Scanner::add_comment_token(TokenClass token_class, unsigned int precedence) {
    // Start with /*, end with */
    NFA* comment_nfa = NFA::from_string("/*");
    // Any char
    NFA* any_char_nfa = NFA::from_any_char();
    any_char_nfa->kleene_star(); // repeat

    comment_nfa->concat(any_char_nfa);
    comment_nfa->concat(NFA::from_string("*/"));
    // Set precedence
    comment_nfa->set_token_class_for_end_state(token_class, precedence);

    // Set precedence
    nfa->set_union(comment_nfa);
}
