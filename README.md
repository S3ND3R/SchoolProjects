# Projects in Introduction to Operating Systems
## Assignment 1 

The goal of this project was to write a simple, command line shell called commando. The shell is less functional than standard shells like bash, but has some properties that distinguish it such as the ability to recall output for any child process. Commando uses a variety of system calls together to accomplish its overall purpose.

Commando covered the following systems programming topics.

Basic C Memory Discipline: A variety of strings and structs are allocated and de-allocated during execution which will require attention to detail and judicious use of memory tools like Valgrind.

fork() and exec(): Text entered that is not recognized as a built-in is treated as an command (external program) to be executed. This spawns a child process which executes the new program.

Pipes, dup2(), read(): Rather than immediately print child output to the screen, child output is redirected into pipes and then retrieved on request by commando.

wait() and waitpid(), blocking and nonblocking: Child processes usually take a while to finish so the shell will check on their status every so often
