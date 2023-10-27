## Operating Systems 238P - UC Irvine, Prof. Tony Givargis

### A repo for all projects of the Operating Systems course.

1. **JIT Compiled Expression Evaluator**: Design a program that accepts a mathematical expression (a string containing the symbols: +, -, *, /, %, (, ), value). This program will dynamically generate a C program based on the input string, invoke a C compiler to create an equivalent loadable module, and then load and execute the machine code equivalent of the C program to produce the evaluated result of the expression.

2. **Userspace Dynamic Thread Scheduler**: Develop a dynamic thread scheduler library with an API akin to the POSIX pthread library, providing the capability to create threads and enable cooperative concurrent execution among them. You can earn 3% extra-credit if you implement an automatic context switch (e.g., every second) from one thread to another without the need for the user threads to call scheduler_yield().

