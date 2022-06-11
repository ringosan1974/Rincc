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

assert 5 "a = 1 + 1;
b = 2 + 1;
a + b;
"
echo OK
