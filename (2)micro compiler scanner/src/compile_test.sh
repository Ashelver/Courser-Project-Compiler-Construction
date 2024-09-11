#!bin/bash

make all

for test_idx in {0..5}; do
    test_lex="../test${test_idx}_lex.txt"
    test_sca="../test${test_idx}_sca.txt"
    testfile="../testcases/test$test_idx.oat"
    echo "test $test_idx"
    ./lexer < $testfile > $test_lex
    ./scanner $testfile > $test_sca
    # if diff -q $test_lex $test_sca; then
    #     echo "|--Pass"
    # else
    #     echo "|--Failed"
    # fi
done

# make clean