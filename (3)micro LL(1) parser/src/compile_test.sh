#!bin/bash

make all

for test_idx in {0..4}; do
    test_result="../test${test_idx}_result.txt"
    testfile="../testcases/test$test_idx-tokens.txt"
    echo "test $test_idx"
    ./parser $testfile > $test_result
done


# make clean