# OS-Assignments
Assignments for course CS F372 Operating Systems at BITS Pilani, Hyderabad Campus.

## List of Assignments

### [Assignment 1](assignment1/)
Process creation, execution, IPC

#### Problem 1: 
It uses `execlp` to execute binaries in the path. So most of the functions that you use in your terminal work here. Also, enjoy the colors.

#### Problem 2:
Fun Question. I've added the modified files.
<br>Here `~` indicates the path where you've extracted the kernel code.
 - `fork.c` :       ~/kernel/fork.c
 - `syscalls.h`:    ~/include/linux/syscalls.h
 - `syscall_64.tbl`:  ~/arch/x86/entry/syscall_64.tbl

#### Problem 3:
It implements a kind of an IPC via pipes which alternates between parent and child. The switching is taken care by the scheduler based on sleep calls in both processes.

#### Problem 4:
Master process spawns a child which remains persistent throughout the program execution. New children are spawned to execute `execlp` whenever required. Message queue has been implemented to enable synchronized communication between the master and the child process.

#### Problem 5:
Simple IPC via pipes alongwith `execlp` to implement newgrep.
P.S. Add `/path/to/newgrep/` to PATH before running executable.
