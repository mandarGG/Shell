You may compile the code normally by typing gcc shell.c and then ./a.out for executing the generated executable file

Features of this shell:
 
 1.Background and foreground process launching

 2.Input/Output redirection functionality

 3.Command redirection using pipes

 4.I/O redirection + pipes (combined usage)

Some other features of this shell are:
 
5.jobs : prints a list of all currently running jobs along with their pid, particularly background jobs, in
order of their creation.

6.kjob <jobNumber> <signalNumber> : takes the job id of a running job and sends a signal value to
that process

7.fg <jobNumber>: brings background job with given job number to foreground.

8.overkill: kills all background process at once

9.quit : exits the shell. Shell exits only if the user types this "quit" command. It
ignores any other signal from user like : CTRL-D, CTRL-C, SIGINT, SIGCHLD etc.

10.CTRL-Z : It changes the status of currently running job to stop, and pushes it in background
process


Some key concepts and commands used here are:  getenv, signal, dup2, wait, waitpid, getpid, kill, execvp, malloc, strtok, fork
                                               Usage of exec family of commands to execute system calls
                                               fork() for creating child processes where needed and wait() for handling child processes
                                               signal handlers to process signals from exiting background processes

The user has the freedom of typing the command anywhere in the command line i.e., by giving spaces, tabs etc.The parser will take care of it for you!!
