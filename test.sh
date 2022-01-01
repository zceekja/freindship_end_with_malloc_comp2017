#!/bin/bash

# Trigger all your test cases with this script

declare -i x=1

declare -i y=0

# Test valid parser
for i in my_tests/test_binary/*
do
    name=${i%.*}
    echo Test $x $name
    prefix="my_tests/test_binary/"
    name=${name#"$prefix"}
    echo Expected:
    cat my_tests/test_expected/$name.out
    echo Observed:
    ./my_tests/test_binary/$name

    a=$(diff <(./my_tests/test_binary/$name) <(cat my_tests/test_expected/$name.out))
    if [[ $a == "" ]]
    then
	    echo "-------------------------------------------------------"
	    echo "	Passed!, result matched!"
	    echo "-------------------------------------------------------"
        y=$(( y + 1 ))
    else
	    echo "-------------------------------------------------------"	
	    echo "	Failed!, Output not matched"
	    echo "-------------------------------------------------------"
    fi	
    x=$(( x + 1 ))

    sleep 1
done
x=$(( x - 1 ))
echo "  End of the test, Summary PASS( $y/$x )"
echo "-------------------------------------------------------"

