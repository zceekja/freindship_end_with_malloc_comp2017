#!/bin/bash

declare -i x=1

# Test maker
for i in my_tests/test_src/*c
do
    name=${i%.*}
    prefix="my_tests/test_src/"
    name=${name#"$prefix"}
    echo Making Test $x $name
    gcc $i virtual_alloc.c virtual_helper.c -o my_tests/test_binary/$name
    x=$(( x + 1 ))
    sleep 0.5
done
