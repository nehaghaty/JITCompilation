#### Valgrind

Command:
```
valgrind --leak-check=full --log-file=valgrind-out.txt ./cs238
```

[Output:](https://github.com/nehaghaty/OperatingSystemsProjects/blob/main/p2/valgrind-out.txt)


#### Strace

Command:
```
strace -o strace_output.txt ./cs238 10+14> stdout.txt 2> stderr.txt
```
[Output:](https://github.com/nehaghaty/OperatingSystemsProjects/blob/main/p2/strace_output.txt)
