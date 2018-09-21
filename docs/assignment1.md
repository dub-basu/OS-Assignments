# [Assignment 1](/src/assignment1/)
Process creation, execution, IPC

## Problems

1. [Problem 1](/src/assignment1/problem1) - Create your own shell<br/>
It uses `execlp` to execute binaries in the path. So most of the functions that you use in your terminal work here. Also, enjoy the colors. 😉

2. [Problem 2](/src/assignment1/problem2) - Add syscall to kernel<br/>
Fun Question. Modified linux kernel version 4.18.2 to create a new syscall.
The following files were modified.
<br>Here `~` indicates the path where the kernel code was extracted.
    - `fork.c` :       ~/kernel/fork.c
    - `syscalls.h`:    ~/include/linux/syscalls.h
    - `syscall_64.tbl`:  ~/arch/x86/entry/syscall_64.tbl

3. [Problem 3](/src/assignment1/problem3)<br/>
It implements a kind of an IPC via pipes which alternates between parent and child. The switching is taken care by the scheduler based on sleep calls in both processes.

4. [Problem 4](/src/assignment1/problem4)<br/>
Master process spawns a child which remains persistent throughout the program execution. New children are spawned to execute `execlp` whenever required. Message queue has been implemented to enable synchronized communication between the master and the child process.

5. [Problem 5](/src/assignment1/problem5)<br/>
Simple IPC via pipes alongwith `execlp` to implement newgrep.
P.S. Add `/path/to/newgrep/` to PATH before running executable.