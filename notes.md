These notes are taken from Modern Operating Systems, 2nd edition by Andrew S. Tanenbaum.

<hr /> 

# Processes and Threads
<sup>Page 53 - 112 </sup>

## Processes
**2.1 -**
A computer can do several things at a time. A CPU can jump from program to program several times a second, executing each for milliseconds at a time. Whilst it can only work on one task at a given moment, a processor can change between them and give an impression of parallelism (pseudoparallelism - an imitation of the parallelism done by multiprocessors that have two or more CPUs and share simultaneously the same physical memory).

**2.1.1 -**
In such a model, all software executables are organized in sequential processes. A process is a program in execution, accompanied by the current value of the program counter (PC), registers and variables. The mechanism of rapid transition between processes is called **multiprogramming**.

Each process has its own control flow. When it is executed, this virtual program counter is loaded into the real program counter. When the allocated CPU time for a process ends, the physical program counter is saved into the logical program counter of the process in memory. 

A program is the code stored in memory. It's static - think of it as a recipe. The process is the execution of said program and can change states during its runtime.

**2.1.2 -**
In general purpose operating systems, a way of creating and terminating processes during operation is necessary when needed. There are four main events that lead to the creation of a process:

1. OS initialization.
2. Execution of a system call to create a process by another process in execution.
3. A user requisition to create a new process.
4. Start of a job in (portuguese: lote?).



