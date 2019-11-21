#!/bin/bash
assert() {
  expected="$1"
  input="$2"

  ./9cc "$input" > tmp.s
  gcc -o tmp tmp.s
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but got $actual"
    exit 1
fi
}

#---- test cases ----
assert 0 '0;'
assert 42 '42;'
assert 21 '5+20-4;'
assert 41 ' 12 + 34 - 5 ;'
assert 47 '5+6*7;'
assert 15 '5*(4-1);'
assert 6 '(4+8)/2;'
assert 10  '-10+20;'
assert 1 '- -1;'
assert 1 '- - +1;'
assert 0 '0==1;'
assert 1 '4==4;'
assert 1 '0!=1;'
assert 1 '2<3;'
assert 0 '3<3;'
assert 1 '2<=2;'
assert 0 '2>3;'
assert 0 '2>=3;'
echo OK