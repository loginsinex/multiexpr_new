# multiexpr
Universal parser for math-like expressions

This implementation of parser parses complex numbers expressions.

For example:

  $ ./mexpr 'a = 1; b = 2; c = 3; x = ( -b - sqrt(b^2 - 4ac) ) / (2a); ax^2 + bx + c'
  
  a = 1; b = 2; c = 3; x = ( -b - sqrt(b^2 - 4ac) ) / (2a); ax^2 + bx + c = -0.000000
  

  $ ./mexpr 'x=pi()/6; arcsin(2sin(x)cos(x))/pi()'
  
  x=pi()/6; arcsin(2sin(x)cos(x))/pi() = 0.333333
  
  
Compile:

  $ cd mexpr
  
  $ make
