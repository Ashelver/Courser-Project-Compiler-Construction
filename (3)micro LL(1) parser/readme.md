## Project structure

    The structure of my workspace:
    .
    ├── testcases
    └── src
        ├── Makefile 
        ├── parser.cpp
        ├── parser.hpp
        ├── compile_test.sh
        └── main.cpp

## How to execute the parser

Uplaod the whole file to the cluster and use the following commands:
```bash
cd path/to/project
cd src
bash compile_test.sh
```
Then you will see 5 result files in the `./` directory
```
report
/src
/testcases
test0_result.txt
test1_result.txt
test2_result.txt
test3_result.txt
test4_result.txt
```

## The format of the results
```
Start Parsing:

           Stack: $ prog
Processed Inputs:
   Current Input: int
            Rule: prog --> decl prog


           Stack: $ prog decl
Processed Inputs:
   Current Input: int
            Rule: decl --> fdecl


......


           Stack: $ prog
Processed Inputs: int id ( ) { var id = stringliteral ; id ( id ) ; return intliteral ; }
   Current Input: $
            Rule: prog --> ε


           Stack: $
Processed Inputs: int id ( ) { var id = stringliteral ; id ( id ) ; return intliteral ; }
   Current Input: $
            Rule: Match $


Accept!

```
Here `Stack` means the stack of the symbols, `Processed Inputs` means the tokens it has processed, `Current Input` means the token it is dealing with, `Rule` means the next action it will do according to the parsing table.


## Q1. Resolve Ambiguity for Micro Language’s Grammar

<div align =center><img src = "image.png" width = 100% height = 100%><p></div>

1. To demonstrate the ambiguity of Micro's grammar, consider the input string "A = B + C + D". This input can be parsed in two different ways:

<div align =center><img src = "image-2.png" width = 60% height = 60%><p>Parse 1</p></div>

<div align =center><img src = "image-3.png" width = 60% height = 60%><p>Parse 2</p></div>

Here, we can have the above different parse way.

<div align =center><img src = "image-1.png" width = 100% height = 100%><p></div>

2. 
   - (a) No. Multiple defined entries in the LL(1) parsing table indicate a conflict in the parsing process, but not necessarily ambiguity. It could be due to FIRST/FOLLOW set conflicts.
   - (b) No. Absence of multiply defined entries in the LL(1) parsing table does not guarantee the absence of ambiguity. It only indicates that the grammar can be parsed using LL(1) parsing technique, which resolves immediate leftmost derivations. Ambiguity might still exist in other parsing techniques or deeper derivations.


## Q2: Simple LL(1) and LR(0) Parsing Exercises

### LL(1) Grammar

<div align =center><img src = "image-4.png" width = 100% height = 100%><p></div>

<div align =center><img src = "image-5.png" width = 100% height = 100%><p></div>

From the web demo, we can find out that T and E are multiply defined.

<div align =center><img src = "image-6.png" width = 60% height = 60%><p></div>

To make the grammar LL(1), we need to eliminate left factor (Here, we don't need to consider the left recursion) of the productions. Here's the modified grammar:


Original Grammar:
```
E → T | T + E
T → int | int * T | ( E )
```

Modified Grammar (eliminate left factor):
```
E → T E'
E' → + E | ε
T → int T' | ( E )
T' → * T | ε
```

Grammar for web demo:
```
E ::= T E'
E' ::= + E
E' ::= ''
T ::= int T'
T ::= ( E )
T' ::= * T
T' ::= ''
```

<div align =center><img src = "image-9.png" width = 100% height = 100%><p></div>


The screenshots of the two tables:
<div align =center><img src = "image-8.png" width = 70% height = 70%><p></div>

<div align =center><img src = "image-10.png" width = 100% height = 100%><p></div>

The screenshots of the parse tree:
<div align =center><img src = "image-7.png" width = 70% height = 70%><p></div>

### LR(0) Grammar

<div align =center><img src = "image-11.png" width = 100% height = 100%><p></div>

The screenshot of the table and DFA diagram:

<div align =center><img src = "image-12.png" width = 90% height = 90%><p></div>


<div align =center><img src = "image-13.png" width = 100% height = 100%><p></div>

The LR(0) parsing table typically fails due to shift-reduce or reduce-reduce conflicts. In the case of the given LL(1) grammar, when constructing the LR(0) parsing table, a reduce-reduce conflict occurs.

Specifically, when constructing the LR(0) parsing table, there would be a conflict in a state where both a reduction and another reduction are possible. This happens because LR(0) parsing doesn't have lookahead, so it can't decide which action to take solely based on the current state.

In the modified LL(1) grammar:
```
E → T E'
E' → + T E' | ε
T → int T'
T' → * T | ε
```
Consider the state in LR(0) parsing when we have already seen "int + int". At this point, the LR(0) parser cannot decide whether to reduce E' to ε or T' to ε since both reductions are possible without any lookahead. This ambiguity leads to a reduce-reduce conflict in the LR(0) parsing table, rendering the grammar not LR(0).


## Q3: Implement LL(1) Parser by hand for Oat v.1

How  to compile and execute my program to get expected output has been attached at the beginning of the report. Here is a simple structure of my LL(1) parser class:

```c++
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
```