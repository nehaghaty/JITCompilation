## Operating Systems 238P - UC Irvine, Prof. Tony Givargis

### A repo for all projects of the Operating Systems course.

1. **JIT Compiled Expression Evaluator**: Design a program that accepts a mathematical expression (a string containing the symbols: +, -, *, /, %, (, ), value). This program will dynamically generate a C program based on the input string, invoke a C compiler to create an equivalent loadable module, and then load and execute the machine code equivalent of the C program to produce the evaluated result of the expression.

2. **Userspace Dynamic Thread Scheduler**: Develop a dynamic thread scheduler library with an API akin to the POSIX pthread library, providing the capability to create threads and enable cooperative concurrent execution among them.

3. **Storage Class Memory Manager**: Create a robust memory management system featuring an API reminiscent of the C malloc() function. This system will utilize a file as its underlying storage to ensure persistent data availability across different processes.

4. **Key/Value File System**: Develop a key/value file system that utilizes raw and direct I/O operations on a block device. Deploy your system on a loop-back device. Ensure that your implementation avoids reliance on kernel buffers for both reading and writing by incorporating efficient write buffering and read caching mechanisms to optimize performance.

5. **System Performance Monitor**: Create a real-time system monitoring tool similar to the Unix top command that provides two different performance statistics in two different categories (e.g., CPU, memory, network, I/O). 
