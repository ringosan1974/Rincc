#!/bin/sh
assert() {
    expected="$1"
    input="$2"
    
    ./rincc "$input" > tmp.s
    cc -o tmp tmp.s
    ./tmp
    actual="$?"
    
    if [ "$actual" = "$expected" ]; then
        echo "$input => $actual"
    else
        echo "$input => $expected expected, but got $actual"
        exit 1
    fi
}

assert 21 "1 + 5 * 4"
assert 3 "( 1 + 5 ) / 2"

echo OK
