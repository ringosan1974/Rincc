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

assert 2 "foo = 1;
bar = 1;
foo + bar;
"
assert 4 "foo = 1 + 1;
bar = 1 + 1;
foo + bar;
"
assert 10 "foo = 2 + 3;
bar = foo * 2;
bar;
"

echo OK
