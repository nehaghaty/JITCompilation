#### Valgrind

Command:
```
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=valgrind-out.txt ./cs238 10+14
```

[Output:](https://github.com/nehaghaty/OperatingSystemsProjects/blob/main/p1/valgrind-out.txt)
```
==9306== Memcheck, a memory error detector
==9306== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.
==9306== Using Valgrind-3.18.1-42b08ed5bd-20211015 and LibVEX; rerun with -h for copyright info
==9306== Command: ./cs238 10+14
==9306== Parent PID: 3308
==9306== 
--9306-- 
--9306-- Valgrind options:
--9306--    --leak-check=full
--9306--    --show-leak-kinds=all
--9306--    --track-origins=yes
--9306--    --verbose
--9306--    --log-file=valgrind-out.txt
--9306-- Contents of /proc/version:
--9306--   Linux version 5.15.0-86-generic (buildd@lcy02-amd64-086) (gcc (Ubuntu 11.4.0-1ubuntu1~22.04) 11.4.0, GNU ld (GNU Binutils for Ubuntu) 2.38) #96-Ubuntu SMP Wed Sep 20 08:23:49 UTC 2023
--9306-- 
--9306-- Arch and hwcaps: AMD64, LittleEndian, amd64-cx16-sse3
--9306-- Page sizes: currently 4096, max supported 4096
--9306-- Valgrind library directory: /usr/libexec/valgrind
--9306-- Reading syms from /home/nghatysa/projects/p1/OperatingSystemsProjects/p1/cs238
--9306-- Reading syms from /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
--9306--   Considering /usr/lib/debug/.build-id/97/18d3757f00d2366056830aae09698dbd35e32c.debug ..
--9306--   .. build-id is valid
--9306-- Reading syms from /usr/libexec/valgrind/memcheck-amd64-linux
--9306--    object doesn't have a symbol table
--9306--    object doesn't have a dynamic symbol table
--9306-- Scheduler: using generic scheduler lock implementation.
--9306-- Reading suppressions file: /usr/libexec/valgrind/default.supp
==9306== embedded gdbserver: reading from /tmp/vgdb-pipe-from-vgdb-to-9306-by-nghatysa-on-???
==9306== embedded gdbserver: writing to   /tmp/vgdb-pipe-to-vgdb-from-9306-by-nghatysa-on-???
==9306== embedded gdbserver: shared mem   /tmp/vgdb-pipe-shared-mem-vgdb-9306-by-nghatysa-on-???
==9306== 
==9306== TO CONTROL THIS PROCESS USING vgdb (which you probably
==9306== don't want to do, unless you know exactly what you're doing,
==9306== or are doing some strange experiment):
==9306==   /usr/bin/vgdb --pid=9306 ...command...
==9306== 
==9306== TO DEBUG THIS PROCESS USING GDB: start GDB like this
==9306==   /path/to/gdb ./cs238
==9306== and then give GDB the following command
==9306==   target remote | /usr/bin/vgdb --pid=9306
==9306== --pid is optional if only one valgrind process is running
==9306== 
--9306-- REDIR: 0x402aa40 (ld-linux-x86-64.so.2:strlen) redirected to 0x580bcec2 (???)
--9306-- REDIR: 0x402a810 (ld-linux-x86-64.so.2:index) redirected to 0x580bcedc (???)
--9306-- Reading syms from /usr/libexec/valgrind/vgpreload_core-amd64-linux.so
--9306--    object doesn't have a symbol table
--9306-- Reading syms from /usr/libexec/valgrind/vgpreload_memcheck-amd64-linux.so
--9306--    object doesn't have a symbol table
==9306== WARNING: new redirection conflicts with existing -- ignoring it
--9306--     old: 0x0402aa40 (strlen              ) R-> (0000.0) 0x580bcec2 ???
--9306--     new: 0x0402aa40 (strlen              ) R-> (2007.0) 0x0484ee30 strlen
--9306-- REDIR: 0x4027220 (ld-linux-x86-64.so.2:strcmp) redirected to 0x484fcd0 (strcmp)
--9306-- REDIR: 0x402afa0 (ld-linux-x86-64.so.2:mempcpy) redirected to 0x4853840 (mempcpy)
--9306-- Reading syms from /usr/lib/x86_64-linux-gnu/libm.so.6
--9306--   Considering /usr/lib/debug/.build-id/d2/c7d1fdefc7a876b6017c090ccd55fb21e8d77f.debug ..
--9306--   .. build-id is valid
--9306-- Reading syms from /usr/lib/x86_64-linux-gnu/libc.so.6
--9306--   Considering /usr/lib/debug/.build-id/a4/3bfc8428df6623cd498c9c0caeb91aec9be4f9.debug ..
--9306--   .. build-id is valid
==9306== WARNING: new redirection conflicts with existing -- ignoring it
--9306--     old: 0x049edc60 (memalign            ) R-> (1011.0) 0x0484e080 memalign
--9306--     new: 0x049edc60 (memalign            ) R-> (1017.0) 0x0484e050 aligned_alloc
==9306== WARNING: new redirection conflicts with existing -- ignoring it
--9306--     old: 0x049edc60 (memalign            ) R-> (1011.0) 0x0484e080 memalign
--9306--     new: 0x049edc60 (memalign            ) R-> (1017.0) 0x0484e020 aligned_alloc
==9306== WARNING: new redirection conflicts with existing -- ignoring it
--9306--     old: 0x049edc60 (memalign            ) R-> (1011.0) 0x0484e080 memalign
--9306--     new: 0x049edc60 (memalign            ) R-> (1017.0) 0x0484e050 aligned_alloc
==9306== WARNING: new redirection conflicts with existing -- ignoring it
--9306--     old: 0x049edc60 (memalign            ) R-> (1011.0) 0x0484e080 memalign
--9306--     new: 0x049edc60 (memalign            ) R-> (1017.0) 0x0484e020 aligned_alloc
--9306-- REDIR: 0x49f0720 (libc.so.6:strnlen) redirected to 0x483f220 (_vgnU_ifunc_wrapper)
--9306-- REDIR: 0x49f09e0 (libc.so.6:strpbrk) redirected to 0x483f220 (_vgnU_ifunc_wrapper)
--9306-- REDIR: 0x49f02e0 (libc.so.6:strcmp) redirected to 0x483f220 (_vgnU_ifunc_wrapper)
--9306-- REDIR: 0x4a0ecd0 (libc.so.6:wcsnlen) redirected to 0x483f220 (_vgnU_ifunc_wrapper)
--9306-- REDIR: 0x49f16d0 (libc.so.6:memset) redirected to 0x483f220 (_vgnU_ifunc_wrapper)
--9306-- REDIR: 0x4a0d540 (libc.so.6:wcslen) redirected to 0x483f220 (_vgnU_ifunc_wrapper)
--9306-- REDIR: 0x49f1c10 (libc.so.6:memcpy@@GLIBC_2.14) redirected to 0x483f220 (_vgnU_ifunc_wrapper)
--9306-- REDIR: 0x4a0d370 (libc.so.6:wcschr) redirected to 0x483f220 (_vgnU_ifunc_wrapper)
--9306-- REDIR: 0x49f0260 (libc.so.6:index) redirected to 0x483f220 (_vgnU_ifunc_wrapper)
--9306-- REDIR: 0x49f0960 (libc.so.6:rindex) redirected to 0x483f220 (_vgnU_ifunc_wrapper)
--9306-- REDIR: 0x49f15a0 (libc.so.6:memmove) redirected to 0x483f220 (_vgnU_ifunc_wrapper)
--9306-- REDIR: 0x4a0d3f0 (libc.so.6:wcscmp) redirected to 0x483f220 (_vgnU_ifunc_wrapper)
--9306-- REDIR: 0x49f19e0 (libc.so.6:stpncpy) redirected to 0x483f220 (_vgnU_ifunc_wrapper)
--9306-- REDIR: 0x4a0d9b0 (libc.so.6:wmemchr) redirected to 0x483f220 (_vgnU_ifunc_wrapper)
--9306-- REDIR: 0x49f0830 (libc.so.6:strncmp) redirected to 0x483f220 (_vgnU_ifunc_wrapper)
--9306-- REDIR: 0x49f1a70 (libc.so.6:strcasecmp) redirected to 0x483f220 (_vgnU_ifunc_wrapper)
--9306-- REDIR: 0x49f0420 (libc.so.6:strcspn) redirected to 0x483f220 (_vgnU_ifunc_wrapper)
--9306-- REDIR: 0x4a0d470 (libc.so.6:wcscpy) redirected to 0x483f220 (_vgnU_ifunc_wrapper)
--9306-- REDIR: 0x49f01d0 (libc.so.6:strcat) redirected to 0x483f220 (_vgnU_ifunc_wrapper)
--9306-- REDIR: 0x49f1b60 (libc.so.6:strncasecmp_l) redirected to 0x483f220 (_vgnU_ifunc_wrapper)
--9306-- REDIR: 0x49f1480 (libc.so.6:bcmp) redirected to 0x483f220 (_vgnU_ifunc_wrapper)
--9306-- REDIR: 0x49f9570 (libc.so.6:memrchr) redirected to 0x483f220 (_vgnU_ifunc_wrapper)
--9306-- REDIR: 0x49f3010 (libc.so.6:strchrnul) redirected to 0x483f220 (_vgnU_ifunc_wrapper)
--9306-- REDIR: 0x49f0390 (libc.so.6:strcpy) redirected to 0x483f220 (_vgnU_ifunc_wrapper)
--9306-- REDIR: 0x49f1b10 (libc.so.6:strcasecmp_l) redirected to 0x483f220 (_vgnU_ifunc_wrapper)
--9306-- REDIR: 0x49f06a0 (libc.so.6:strlen) redirected to 0x483f220 (_vgnU_ifunc_wrapper)
--9306-- REDIR: 0x49f08d0 (libc.so.6:strncpy) redirected to 0x483f220 (_vgnU_ifunc_wrapper)
--9306-- REDIR: 0x49f1950 (libc.so.6:stpcpy) redirected to 0x483f220 (_vgnU_ifunc_wrapper)
--9306-- REDIR: 0x49f13f0 (libc.so.6:memchr) redirected to 0x483f220 (_vgnU_ifunc_wrapper)
--9306-- REDIR: 0x49f0b00 (libc.so.6:strspn) redirected to 0x483f220 (_vgnU_ifunc_wrapper)
--9306-- REDIR: 0x49f17d0 (libc.so.6:mempcpy) redirected to 0x483f220 (_vgnU_ifunc_wrapper)
--9306-- REDIR: 0x49f1ac0 (libc.so.6:strncasecmp) redirected to 0x483f220 (_vgnU_ifunc_wrapper)
--9306-- REDIR: 0x49f2f80 (libc.so.6:rawmemchr) redirected to 0x483f220 (_vgnU_ifunc_wrapper)
--9306-- REDIR: 0x4a02650 (libc.so.6:__strrchr_sse2) redirected to 0x484e870 (__strrchr_sse2)
--9306-- REDIR: 0x4a02950 (libc.so.6:__strlen_sse2) redirected to 0x484ed70 (__strlen_sse2)
--9306-- REDIR: 0x49ee520 (libc.so.6:calloc) redirected to 0x484d9d0 (calloc)
--9306-- REDIR: 0x49ed740 (libc.so.6:realloc) redirected to 0x484dc50 (realloc)
--9306-- REDIR: 0x49ed0a0 (libc.so.6:malloc) redirected to 0x4848820 (malloc)
--9306-- REDIR: 0x49ed3e0 (libc.so.6:free) redirected to 0x484b210 (free)
--9306-- REDIR: 0x49f0fc0 (libc.so.6:__GI_strstr) redirected to 0x4853ae0 (__strstr_sse2)
--9306-- REDIR: 0x4a02440 (libc.so.6:__strchrnul_sse2) redirected to 0x4853330 (strchrnul)
--9306-- REDIR: 0x4a0c850 (libc.so.6:__mempcpy_sse2_unaligned) redirected to 0x4853440 (mempcpy)
--9306-- REDIR: 0x4a0c870 (libc.so.6:memcpy@GLIBC_2.2.5) redirected to 0x484fe00 (memcpy@GLIBC_2.2.5)
--9306-- Reading syms from /home/nghatysa/projects/p1/OperatingSystemsProjects/p1/out.so
--9306-- Discarding syms at 0x485c040-0x485c10e in /home/nghatysa/projects/p1/OperatingSystemsProjects/p1/out.so (have_dinfo 1)
==9306== 
==9306== HEAP SUMMARY:
==9306==     in use at exit: 0 bytes in 0 blocks
==9306==   total heap usage: 16 allocs, 16 frees, 11,011 bytes allocated
==9306== 
==9306== All heap blocks were freed -- no leaks are possible
==9306== 
==9306== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
```

#### Strace

Command:
```
strace -o strace_output.txt ./cs238 10+14> stdout.txt 2> stderr.txt
```
[Output:](https://github.com/nehaghaty/OperatingSystemsProjects/blob/main/p1/strace_output.txt)

```
execve("./cs238", ["./cs238", "10+14"], 0x7ffffbf8a6e8 /* 25 vars */) = 0
brk(NULL)                               = 0x560a3a8e1000
arch_prctl(0x3001 /* ARCH_??? */, 0x7ffd4dd35bc0) = -1 EINVAL (Invalid argument)
mmap(NULL, 8192, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0) = 0x7fa00d6d5000
access("/etc/ld.so.preload", R_OK)      = -1 ENOENT (No such file or directory)
openat(AT_FDCWD, "/home/nghatysa/projects/p1/OperatingSystemsProjects/p1/tls/x86_64/x86_64/libm.so.6", O_RDONLY|O_CLOEXEC) = -1 ENOENT (No such file or directory)
newfstatat(AT_FDCWD, "/home/nghatysa/projects/p1/OperatingSystemsProjects/p1/tls/x86_64/x86_64", 0x7ffd4dd34de0, 0) = -1 ENOENT (No such file or directory)
openat(AT_FDCWD, "/home/nghatysa/projects/p1/OperatingSystemsProjects/p1/tls/x86_64/libm.so.6", O_RDONLY|O_CLOEXEC) = -1 ENOENT (No such file or directory)
newfstatat(AT_FDCWD, "/home/nghatysa/projects/p1/OperatingSystemsProjects/p1/tls/x86_64", 0x7ffd4dd34de0, 0) = -1 ENOENT (No such file or directory)
openat(AT_FDCWD, "/home/nghatysa/projects/p1/OperatingSystemsProjects/p1/tls/x86_64/libm.so.6", O_RDONLY|O_CLOEXEC) = -1 ENOENT (No such file or directory)
newfstatat(AT_FDCWD, "/home/nghatysa/projects/p1/OperatingSystemsProjects/p1/tls/x86_64", 0x7ffd4dd34de0, 0) = -1 ENOENT (No such file or directory)
openat(AT_FDCWD, "/home/nghatysa/projects/p1/OperatingSystemsProjects/p1/tls/libm.so.6", O_RDONLY|O_CLOEXEC) = -1 ENOENT (No such file or directory)
newfstatat(AT_FDCWD, "/home/nghatysa/projects/p1/OperatingSystemsProjects/p1/tls", 0x7ffd4dd34de0, 0) = -1 ENOENT (No such file or directory)
openat(AT_FDCWD, "/home/nghatysa/projects/p1/OperatingSystemsProjects/p1/x86_64/x86_64/libm.so.6", O_RDONLY|O_CLOEXEC) = -1 ENOENT (No such file or directory)
newfstatat(AT_FDCWD, "/home/nghatysa/projects/p1/OperatingSystemsProjects/p1/x86_64/x86_64", 0x7ffd4dd34de0, 0) = -1 ENOENT (No such file or directory)
openat(AT_FDCWD, "/home/nghatysa/projects/p1/OperatingSystemsProjects/p1/x86_64/libm.so.6", O_RDONLY|O_CLOEXEC) = -1 ENOENT (No such file or directory)
newfstatat(AT_FDCWD, "/home/nghatysa/projects/p1/OperatingSystemsProjects/p1/x86_64", 0x7ffd4dd34de0, 0) = -1 ENOENT (No such file or directory)
openat(AT_FDCWD, "/home/nghatysa/projects/p1/OperatingSystemsProjects/p1/x86_64/libm.so.6", O_RDONLY|O_CLOEXEC) = -1 ENOENT (No such file or directory)
newfstatat(AT_FDCWD, "/home/nghatysa/projects/p1/OperatingSystemsProjects/p1/x86_64", 0x7ffd4dd34de0, 0) = -1 ENOENT (No such file or directory)
openat(AT_FDCWD, "/home/nghatysa/projects/p1/OperatingSystemsProjects/p1/libm.so.6", O_RDONLY|O_CLOEXEC) = -1 ENOENT (No such file or directory)
newfstatat(AT_FDCWD, "/home/nghatysa/projects/p1/OperatingSystemsProjects/p1", {st_mode=S_IFDIR|0775, st_size=4096, ...}, 0) = 0
openat(AT_FDCWD, "/etc/ld.so.cache", O_RDONLY|O_CLOEXEC) = 3
newfstatat(3, "", {st_mode=S_IFREG|0644, st_size=23727, ...}, AT_EMPTY_PATH) = 0
mmap(NULL, 23727, PROT_READ, MAP_PRIVATE, 3, 0) = 0x7fa00d6cf000
close(3)                                = 0
openat(AT_FDCWD, "/lib/x86_64-linux-gnu/libm.so.6", O_RDONLY|O_CLOEXEC) = 3
read(3, "\177ELF\2\1\1\3\0\0\0\0\0\0\0\0\3\0>\0\1\0\0\0\0\0\0\0\0\0\0\0"..., 832) = 832
newfstatat(3, "", {st_mode=S_IFREG|0644, st_size=940560, ...}, AT_EMPTY_PATH) = 0
mmap(NULL, 942344, PROT_READ, MAP_PRIVATE|MAP_DENYWRITE, 3, 0) = 0x7fa00d5e8000
mmap(0x7fa00d5f6000, 507904, PROT_READ|PROT_EXEC, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0xe000) = 0x7fa00d5f6000
mmap(0x7fa00d672000, 372736, PROT_READ, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x8a000) = 0x7fa00d672000
mmap(0x7fa00d6cd000, 8192, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0xe4000) = 0x7fa00d6cd000
close(3)                                = 0
openat(AT_FDCWD, "/home/nghatysa/projects/p1/OperatingSystemsProjects/p1/libc.so.6", O_RDONLY|O_CLOEXEC) = -1 ENOENT (No such file or directory)
openat(AT_FDCWD, "/lib/x86_64-linux-gnu/libc.so.6", O_RDONLY|O_CLOEXEC) = 3
read(3, "\177ELF\2\1\1\3\0\0\0\0\0\0\0\0\3\0>\0\1\0\0\0P\237\2\0\0\0\0\0"..., 832) = 832
pread64(3, "\6\0\0\0\4\0\0\0@\0\0\0\0\0\0\0@\0\0\0\0\0\0\0@\0\0\0\0\0\0\0"..., 784, 64) = 784
pread64(3, "\4\0\0\0 \0\0\0\5\0\0\0GNU\0\2\0\0\300\4\0\0\0\3\0\0\0\0\0\0\0"..., 48, 848) = 48
pread64(3, "\4\0\0\0\24\0\0\0\3\0\0\0GNU\0\244;\374\204(\337f#\315I\214\234\f\256\271\32"..., 68, 896) = 68
newfstatat(3, "", {st_mode=S_IFREG|0755, st_size=2216304, ...}, AT_EMPTY_PATH) = 0
pread64(3, "\6\0\0\0\4\0\0\0@\0\0\0\0\0\0\0@\0\0\0\0\0\0\0@\0\0\0\0\0\0\0"..., 784, 64) = 784
mmap(NULL, 2260560, PROT_READ, MAP_PRIVATE|MAP_DENYWRITE, 3, 0) = 0x7fa00d3c0000
mmap(0x7fa00d3e8000, 1658880, PROT_READ|PROT_EXEC, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x28000) = 0x7fa00d3e8000
mmap(0x7fa00d57d000, 360448, PROT_READ, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x1bd000) = 0x7fa00d57d000
mmap(0x7fa00d5d5000, 24576, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x214000) = 0x7fa00d5d5000
mmap(0x7fa00d5db000, 52816, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_ANONYMOUS, -1, 0) = 0x7fa00d5db000
close(3)                                = 0
mmap(NULL, 12288, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0) = 0x7fa00d3bd000
arch_prctl(ARCH_SET_FS, 0x7fa00d3bd740) = 0
set_tid_address(0x7fa00d3bda10)         = 9557
set_robust_list(0x7fa00d3bda20, 24)     = 0
rseq(0x7fa00d3be0e0, 0x20, 0, 0x53053053) = 0
mprotect(0x7fa00d5d5000, 16384, PROT_READ) = 0
mprotect(0x7fa00d6cd000, 4096, PROT_READ) = 0
mprotect(0x560a38aec000, 4096, PROT_READ) = 0
mprotect(0x7fa00d70f000, 8192, PROT_READ) = 0
prlimit64(0, RLIMIT_STACK, NULL, {rlim_cur=8192*1024, rlim_max=RLIM64_INFINITY}) = 0
munmap(0x7fa00d6cf000, 23727)           = 0
getrandom("\x57\xc6\x8b\x52\xa9\xa4\xad\x1a", 8, GRND_NONBLOCK) = 8
brk(NULL)                               = 0x560a3a8e1000
brk(0x560a3a902000)                     = 0x560a3a902000
openat(AT_FDCWD, "out.c", O_WRONLY|O_CREAT|O_TRUNC, 0666) = 3
newfstatat(3, "", {st_mode=S_IFREG|0664, st_size=0, ...}, AT_EMPTY_PATH) = 0
write(3, "typedef double (*sigmoid_t)(doub"..., 171) = 171
close(3)                                = 0
clone(child_stack=NULL, flags=CLONE_CHILD_CLEARTID|CLONE_CHILD_SETTID|SIGCHLD, child_tidptr=0x7fa00d3bda10) = 9558
wait4(9558, [{WIFEXITED(s) && WEXITSTATUS(s) == 0}], 0, NULL) = 9558
--- SIGCHLD {si_signo=SIGCHLD, si_code=CLD_EXITED, si_pid=9558, si_uid=1000, si_status=0, si_utime=0, si_stime=0} ---
unlink("out.c")                         = 0
openat(AT_FDCWD, "/home/nghatysa/projects/p1/OperatingSystemsProjects/p1/out.so", O_RDONLY|O_CLOEXEC) = 3
read(3, "\177ELF\2\1\1\0\0\0\0\0\0\0\0\0\3\0>\0\1\0\0\0\0\0\0\0\0\0\0\0"..., 832) = 832
newfstatat(3, "", {st_mode=S_IFREG|0775, st_size=15208, ...}, AT_EMPTY_PATH) = 0
mmap(NULL, 16424, PROT_READ, MAP_PRIVATE|MAP_DENYWRITE, 3, 0) = 0x7fa00d6d0000
mmap(0x7fa00d6d1000, 4096, PROT_READ|PROT_EXEC, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x1000) = 0x7fa00d6d1000
mmap(0x7fa00d6d2000, 4096, PROT_READ, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x2000) = 0x7fa00d6d2000
mmap(0x7fa00d6d3000, 8192, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x2000) = 0x7fa00d6d3000
close(3)                                = 0
mprotect(0x7fa00d6d3000, 4096, PROT_READ) = 0
newfstatat(1, "", {st_mode=S_IFREG|0664, st_size=0, ...}, AT_EMPTY_PATH) = 0
unlink("out.so")                        = 0
munmap(0x7fa00d6d0000, 16424)           = 0
write(1, "1.000000\n", 9)               = 9
exit_group(0)                           = ?
+++ exited with 0 +++
```
