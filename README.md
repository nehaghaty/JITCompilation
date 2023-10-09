# JIT Compiled Expression Evaluator

## Operating Systems 238P, Prof. Tony Givargis

Design a program that accepts a mathematical expression (a string containing the symbols: +, -, *, /, %, (, ), value). This program will dynamically generate a C program based on the input string, invoke a C compiler to create an equivalent loadable module, and then load and execute the machine code equivalent of the C program to produce the evaluated result of the expression.
