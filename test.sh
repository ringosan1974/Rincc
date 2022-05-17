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

assert 2 "1 + 1"
assert 5 "6 - 1"
assert 10 "2 * 5"
assert 3 "6 / 2"
assert 20 "-10 * -2"
assert 1 "1 + 2 == 4 - 1"
assert 0 "1 * 3 == 5 * 2"
assert 1 "1 + 1 != 2 * 2"
assert 0 "10 / 2 != 1 + 4"
assert 1 "2 > 1"
assert 0 "1 > 2"
assert 1 "2 < 3"
assert 1 "2 => 2"
assert 0 "2 => 3"
assert 1 "3 =< 3"

echo OK
