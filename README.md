# Projects in Introduction to Operating Systems
## Assignment 1 

The goal of this project was to write a simple, command line shell called commando. The shell is less functional than standard shells like bash, but has some properties that distinguish it such as the ability to recall output for any child process. Commando uses a variety of system calls together to accomplish its overall purpose.

Commando covered the following systems programming topics.

Basic C Memory Discipline: A variety of strings and structs are allocated and de-allocated during execution which will require attention to detail and judicious use of memory tools like Valgrind.

fork() and exec(): Text entered that is not recognized as a built-in is treated as an command (external program) to be executed. This spawns a child process which executes the new program.

Pipes, dup2(), read(): Rather than immediately print child output to the screen, child output is redirected into pipes and then retrieved on request by commando.

wait() and waitpid(), blocking and nonblocking: Child processes usually take a while to finish so the shell will check on their status every so often

## Assignment 2

This project focused on developing a simple chat server and client called blather.

Server
Some user starts bl_server which manages the chat "room". The server is non-interactive and will likely only print debugging output as it runs.

Client
Users run bl_client which takes input typed on the keyboard and sends it to the server. The server broadcasts the input to all other clients who can respond by typing their own input.

blather utilizes a combination of system tools.

* Multiple communicating processes: clients to servers
* Communication through FIFOs
* Signal handling for graceful server shutdown
* Alarm signals for periodic behavior
* Input multiplexing with poll()
* Multiple threads in the client to handle typed input versus info from the server
